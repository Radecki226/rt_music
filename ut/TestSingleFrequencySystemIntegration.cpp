#include "SingleFrequencyGenerator.hpp"
#include "SingleFrequencySystemIntegration.hpp"
#include "UniformLinearArray.hpp"

#include <catch2/catch_test_macros.hpp>

#include "trompeloeil.hpp"
#include <array>
#include <complex>

TEST_CASE( "SingleFrequencySystemIntegration check peak", "[SingleFrequencySystemIntegration]" ) {

    constexpr float frequencyHz = 1000.0f;
    constexpr float spacingMeters = 0.05f;
    constexpr size_t M = 4;

    struct SingleFrequencyGeneratorConfig generatorConfig = {
        .snr = 20.0f,
        .thetaRad = M_PI / 4.0f,
        .frequency = frequencyHz
    };

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .singleFrequencySystemConfig = {
            .frequencyHz = frequencyHz,
            .nAngles = 360,
            .computeIntervalFrames = 1,
            .nSources = 1
        },
        .nAveragingFrames = 10,
        .spacingMeters = spacingMeters
    };

    UniformLinearArray<M> uniformLinearArray(spacingMeters);
    SingleFrequencyGenerator<M> signalGenerator(uniformLinearArray, generatorConfig);
    SingleFrequencySystemIntegration<M> system(systemConfig);

    std::array<std::complex<float>, M> inputFrame = signalGenerator.generateInput();
    REQUIRE(inputFrame.size() == M);
}
