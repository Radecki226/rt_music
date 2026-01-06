#include <catch2/catch_test_macros.hpp>

#include "SingleFrequencySystem.hpp"
#include "trompeloeil.hpp"
#include <array>
#include <complex>

class MockCircularBuffer : public ICircularBuffer<4> {
public:
    MAKE_MOCK1(push, void(const std::array<std::complex<float>, 4>&), override);
    MAKE_CONST_MOCK1(calcCov, void(Eigen::Matrix<std::complex<float>, 4, 4>&), override);
};

class MockSteeringVectorModel : public ISteeringVectorModel<4> {
public:
    using steeringVectorType = Eigen::Matrix<std::complex<float>, 4, 1>;

    MAKE_CONST_MOCK2(getSteeringVector, steeringVectorType(float, float), override);
};

class MockDspMusic : public IDspMusic<4> {
public:
    MAKE_CONST_MOCK2(computeNoiseSpace, void(Eigen::Matrix<std::complex<float>, 4, Eigen::Dynamic>&,
                                             const Eigen::Matrix<std::complex<float>, 4, 4>&), override);


    MAKE_MOCK2(calculatePseudospectrum, float(const Eigen::Matrix<std::complex<float>, 4, 1>&,
                                                     const Eigen::Matrix<std::complex<float>, 4, Eigen::Dynamic>&), override);
};

static MockCircularBuffer mockCircularBuffer;
static MockSteeringVectorModel mockSteeringVectorModel;
static MockDspMusic mockDspMusic;

TEST_CASE( "SingleFrequencySystem check", "[SingleFrequencySystem]" ) {
    constexpr SingleFrequencySystemConfig config = {
        .frequencyHz = 1000.0f,
        .nAngles = 36,
        .computeIntervalFrames = 3,
        .nSources = 1
    };
    SingleFrequencySystem<4> system(config, mockCircularBuffer, mockSteeringVectorModel, mockDspMusic);

    std::array<std::complex<float>, 4> sample = {
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
    
    REQUIRE_CALL(mockSteeringVectorModel, getSteeringVector(trompeloeil::_, trompeloeil::_))
        .TIMES(config.nAngles)
        .RETURN(Eigen::Matrix<std::complex<float>, 4, 1>::Ones());

    REQUIRE_CALL(mockDspMusic, calculatePseudospectrum(trompeloeil::_, trompeloeil::_))
        .TIMES(config.nAngles)
        .RETURN(13.0f);

    REQUIRE(system.processFrame(sample) == false);
    REQUIRE(system.processFrame(sample) == false);
    REQUIRE(system.processFrame(sample) == true);
    REQUIRE(system.getPseudospectrum().size() == config.nAngles);
    REQUIRE(system.getPseudospectrum()(0) == 13.0f);
}
