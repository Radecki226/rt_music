#include <catch2/catch_test_macros.hpp>

#include "MultiFrequencySystemIntegration.hpp"
#include "MusicConstants.hpp"
#include "SingleFrequencyGenerator.hpp"
#include "UniformPlanarSquareArray.hpp"
#include "CustomMalloc.hpp"

#include <array>
#include <cmath>
#include <complex>
#include <random>


namespace {

float frequencyHzForIndex(size_t freqIdx) {
    return MusicConstants::start_freq + static_cast<float>(freqIdx) * MusicConstants::freq_step;
}

std::array<size_t, 3> pickThreeDistinctFrequencyIndices(std::mt19937 &rng) {
    std::uniform_int_distribution<size_t> uni(0, MusicConstants::n_frequencies - 1);
    std::array<size_t, 3> r{};
    r[0] = uni(rng);
    do {
        r[1] = uni(rng);
    } while (r[1] == r[0]);
    do {
        r[2] = uni(rng);
    } while (r[2] == r[0] || r[2] == r[1]);
    return r;
}

class ThreeTonePerFrequencyFrameGenerator {
public:
    ThreeTonePerFrequencyFrameGenerator(std::mt19937 &rng, float thetaRad, std::array<size_t, 3> freqIdx, float snr)
        : rng_(rng),
          thetaRad_(thetaRad),
          freqIdx_(freqIdx),
          planar_(MusicConstants::spacing_meters),
          gen0_(planar_,
                SingleFrequencyGeneratorConfig{
                    .snr = snr,
                    .thetaRad = thetaRad_,
                    .frequency = frequencyHzForIndex(freqIdx_[0]),
                }),
          gen1_(planar_,
                SingleFrequencyGeneratorConfig{
                    .snr = snr,
                    .thetaRad = thetaRad_,
                    .frequency = frequencyHzForIndex(freqIdx_[1]),
                }),
          gen2_(planar_,
                SingleFrequencyGeneratorConfig{
                    .snr = snr,
                    .thetaRad = thetaRad_,
                    .frequency = frequencyHzForIndex(freqIdx_[2]),
                }) {}

    std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies> next() {
        std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies> out{};

        std::normal_distribution<float> tiny(0.0f, 1e-3f);
        for (size_t i = 0; i < MusicConstants::n_frequencies; ++i) {
            for (size_t m = 0; m < MusicConstants::M; ++m) {
                out[i][m] = {tiny(rng_), tiny(rng_)};
            }
        }

        out[freqIdx_[0]] = gen0_.generateInput();
        out[freqIdx_[1]] = gen1_.generateInput();
        out[freqIdx_[2]] = gen2_.generateInput();
        return out;
    }

    const std::array<size_t, 3> &frequencyIndices() const { return freqIdx_; }

private:
    std::mt19937 &rng_;
    float thetaRad_;
    std::array<size_t, 3> freqIdx_;
    UniformPlanarSquareArray<MusicConstants::M> planar_;
    SingleFrequencyGenerator<MusicConstants::M> gen0_;
    SingleFrequencyGenerator<MusicConstants::M> gen1_;
    SingleFrequencyGenerator<MusicConstants::M> gen2_;
};

}

TEST_CASE("MultiFrequencySystemIntegration three random tones peak near true azimuth",
          "[MultiFrequencySystemIntegration]") {
    constexpr float kTheta = static_cast<float>(M_PI) / 5.0f;
    constexpr float kSnr = 45.0f;

    std::mt19937 rng(42);
    const auto freqIdx = pickThreeDistinctFrequencyIndices(rng);

    ThreeTonePerFrequencyFrameGenerator generator(rng, kTheta, freqIdx, kSnr);

    MultiFrequencySystemIntegrationConfig sysCfg = {
        .computeIntervalFrames = 5,
        .nSources = 1,
        .nAveragingFrames = 5,
    };
    MultiFrequencySystemIntegration<MusicConstants::M> multi(sysCfg);

    bool anyUpdate = false;
    for (int iter = 0; iter < 200; ++iter) {
        if (multi.processFrame(generator.next())) {
            anyUpdate = true;
        }
    }
    REQUIRE(anyUpdate);

    const Eigen::Matrix<float, Eigen::Dynamic, 1> &ps = multi.getPseudospectrum();
    REQUIRE(ps.size() == static_cast<Eigen::Index>(MusicConstants::n_angles));

    Eigen::Index peakIndex = 0;
    float peakVal = ps(0);
    for (Eigen::Index i = 1; i < ps.size(); ++i) {
        if (ps(i) > peakVal) {
            peakVal = ps(i);
            peakIndex = i;
        }
    }

    const float estimatedAngle = static_cast<float>(peakIndex) * MusicConstants::angle_step;
    REQUIRE(std::abs(estimatedAngle - kTheta) < 3.0f * MusicConstants::angle_step);

    for (size_t k = 0; k < 3; ++k) {
        const float hz = frequencyHzForIndex(generator.frequencyIndices()[k]);
        REQUIRE(hz >= MusicConstants::start_freq);
        REQUIRE(hz <= MusicConstants::end_freq);
    }
}

TEST_CASE("MultiFrequencySystemIntegration memory alloc", "[MultiFrequencySystemIntegration]") {
    MultiFrequencySystemIntegrationConfig sysCfg = {
        .computeIntervalFrames = 5,
        .nSources = 1,
        .nAveragingFrames = 5,
    };
    MultiFrequencySystemIntegration<MusicConstants::M> multi(sysCfg);
    std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies> frame;

    CustomMalloc::resetMallocCounter();
    for (int i = 0; i < 5; i++) {
        multi.processFrame(frame);
    }

    REQUIRE(CustomMalloc::getMallocCounter() == 0);
}
