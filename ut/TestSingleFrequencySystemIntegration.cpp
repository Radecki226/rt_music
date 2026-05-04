#include "SingleFrequencyGenerator.hpp"
#include "SingleFrequencySystemIntegration.hpp"
#include "UniformPlanarSquareArray.hpp"
#include "MusicConstants.hpp"
#include "CustomMalloc.hpp"

#include <catch2/catch_test_macros.hpp>

#include "Eigen/Dense"
#include <array>
#include <complex>

TEST_CASE( "SingleFrequencySystemIntegration check peak", "[SingleFrequencySystemIntegration]" ) {
    constexpr float frequencyHz = 1187.5f;

    struct SingleFrequencyGeneratorConfig generatorConfig = {
        .snr = 40.0f,
        .thetaRad = M_PI / 4.0f,
        .frequency = frequencyHz
    };

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .computeIntervalFrames = 5,
        .nSources = 1,
        .nAveragingFrames = 5
    };

    UniformPlanarSquareArray<MusicConstants::M> uniformPlanarSquareArray(MusicConstants::spacing_meters);
    SingleFrequencyGenerator<MusicConstants::M> signalGenerator(uniformPlanarSquareArray, generatorConfig);
    SingleFrequencySystemIntegration<MusicConstants::M> system(systemConfig);


    //Print input frame
    REQUIRE(system.processFrame(signalGenerator.generateInput()) == false);
    REQUIRE(system.processFrame(signalGenerator.generateInput()) == false);
    REQUIRE(system.processFrame(signalGenerator.generateInput()) == false);
    REQUIRE(system.processFrame(signalGenerator.generateInput()) == false);
    REQUIRE(system.processFrame(signalGenerator.generateInput()) == true);

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& pseudospectrum = system.getPseudospectrum();
    REQUIRE(pseudospectrum.size() == MusicConstants::n_angles);

    //Print pseudospectrum values
    size_t peakIndex = 0;
    float peakValue = 0.0f;
    for (size_t i = 0; i < pseudospectrum.size(); ++i) {
        if (pseudospectrum(i) > peakValue) {
            peakValue = pseudospectrum(i);
            peakIndex = i;
        }
    }
    float angleStepRad = MusicConstants::angle_step;
    float estimatedAngleRad = peakIndex * angleStepRad;

    REQUIRE(std::abs(estimatedAngleRad - generatorConfig.thetaRad) < angleStepRad);
}

TEST_CASE( "SingleFrequencySystemIntegration check peak low SNR, wide averaging window", "[SingleFrequencySystemIntegration]" ) {
    constexpr float frequencyHz = 1187.5f;

    struct SingleFrequencyGeneratorConfig generatorConfig = {
        .snr = 16.0f,
        .thetaRad = M_PI / 4.0f,
        .frequency = frequencyHz
    };

    constexpr int kComputeIntervalFrames = 60;

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .computeIntervalFrames = kComputeIntervalFrames,
        .nSources = 1,
        .nAveragingFrames = kComputeIntervalFrames
    };

    UniformPlanarSquareArray<MusicConstants::M> uniformPlanarSquareArray(MusicConstants::spacing_meters);
    SingleFrequencyGenerator<MusicConstants::M> signalGenerator(uniformPlanarSquareArray, generatorConfig);
    SingleFrequencySystemIntegration<MusicConstants::M> system(systemConfig);


    for (int i = 0; i < kComputeIntervalFrames-1; i++) {
        bool result = system.processFrame(signalGenerator.generateInput());
        REQUIRE(result == false);
    }
    bool result = system.processFrame(signalGenerator.generateInput());
    REQUIRE(result == true);

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& pseudospectrum = system.getPseudospectrum();
    REQUIRE(pseudospectrum.size() == MusicConstants::n_angles);

    //Print pseudospectrum values
    size_t peakIndex = 0;
    float peakValue = 0.0f;
    for (size_t i = 0; i < pseudospectrum.size(); ++i) {
        printf("pseudospectrum(%zu) = %f\n", i, pseudospectrum(i));
        if (pseudospectrum(i) > peakValue) {
            peakValue = pseudospectrum(i);
            peakIndex = i;
        }
    }
    float angleStepRad = MusicConstants::angle_step;
    float estimatedAngleRad = peakIndex * angleStepRad;

    REQUIRE(std::abs(estimatedAngleRad - generatorConfig.thetaRad) < angleStepRad);
}


TEST_CASE( "SingleFrequencySystemIntegration check memory alloc", "[SingleFrequencySystemIntegration]" ) {

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .computeIntervalFrames = 5,
        .nSources = 1,
        .nAveragingFrames = 5
    };

    SingleFrequencySystemIntegration<MusicConstants::M> system(systemConfig);
    std::array<std::complex<float>, MusicConstants::M> inputFrame;

    CustomMalloc::resetMallocCounter();

    //Process frames - should not allocate steering vectors at runtime
    for (int i = 0; i < 5; i++) {
        system.processFrame(inputFrame);
    }

    REQUIRE(CustomMalloc::getMallocCounter() == 0);
}
