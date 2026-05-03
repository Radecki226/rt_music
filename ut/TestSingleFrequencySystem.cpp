#include <catch2/catch_test_macros.hpp>

#include "SingleFrequencySystem.hpp"
#include "MusicConstants.hpp"
#include "trompeloeil.hpp"
#include <array>
#include <complex>

class MockCircularBuffer : public ICircularBuffer<MusicConstants::M> {
public:
    MAKE_MOCK1(push, void(const std::array<std::complex<float>, MusicConstants::M>&), override);
    MAKE_CONST_MOCK1(calcCov, void(Eigen::Matrix<std::complex<float>, MusicConstants::M, MusicConstants::M>&), override);
};

class MockDspMusic : public IDspMusic<MusicConstants::M> {
public:
    using PseudospectrumVector = Eigen::Matrix<float, Eigen::Dynamic, 1>;
    using PseudospectrumOut = Eigen::Ref<PseudospectrumVector>;
    using ComplexDynamicMatrix = Eigen::Matrix<std::complex<float>, MusicConstants::M, Eigen::Dynamic>;

    MAKE_CONST_MOCK2(computeNoiseSpace, void(ComplexDynamicMatrix&, const Eigen::Matrix<std::complex<float>, MusicConstants::M, MusicConstants::M>&), override);

    MAKE_MOCK3(calculatePseudospectrumBatch,
               void(const ComplexDynamicMatrix&, const ComplexDynamicMatrix&, PseudospectrumOut),
               override);
};

static MockCircularBuffer mockCircularBuffer;
static MockDspMusic mockDspMusic;

TEST_CASE( "SingleFrequencySystem check", "[SingleFrequencySystem]" ) {
    constexpr SingleFrequencySystemConfig config = {
        .frequencyIdx = 15,  // Index into steering_vectors array
        .nAngles = MusicConstants::n_angles,
        .computeIntervalFrames = 3,
        .nSources = 1
    };
    SingleFrequencySystem<MusicConstants::M> system(config, mockCircularBuffer, mockDspMusic);

    std::array<std::complex<float>, MusicConstants::M> sample = {
        std::complex<float>(1.0f, 0.0f),
        std::complex<float>(0.5f, 0.5f),
        std::complex<float>(0.0f, 1.0f),
        std::complex<float>(-0.5f, 0.5f)
    };

    REQUIRE_CALL(mockCircularBuffer, push(trompeloeil::eq(sample)))
        .TIMES(3);
    
    REQUIRE_CALL(mockCircularBuffer, calcCov(trompeloeil::_))
        .TIMES(1);

    REQUIRE_CALL(mockDspMusic, computeNoiseSpace(trompeloeil::_, trompeloeil::_))
        .TIMES(1);
    
    // Note: No call to getSteeringVector - using precomputed steering vectors instead
    REQUIRE_CALL(mockDspMusic, calculatePseudospectrumBatch(trompeloeil::_, trompeloeil::_, trompeloeil::_))
        .TIMES(1)
        .SIDE_EFFECT(_3.setConstant(13.0f));

    REQUIRE(system.processFrame(sample) == false);
    REQUIRE(system.processFrame(sample) == false);
    REQUIRE(system.processFrame(sample) == true);
    REQUIRE(system.getPseudospectrum().size() == config.nAngles);
    REQUIRE(system.getPseudospectrum()(0) == 13.0f);
}
