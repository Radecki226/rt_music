#include "SingleFrequencyGenerator.hpp"
#include "SingleFrequencySystemIntegration.hpp"
#include "UniformLinearArray.hpp"

#include <catch2/catch_test_macros.hpp>

#include "trompeloeil.hpp"
#include <array>
#include <complex>
#include <cstdio>

#include <dlfcn.h>
#include <unistd.h>

static size_t mallocCounter = 0;

extern "C" {
    void* malloc(size_t size) noexcept {
        // This is the "assignment" part: find the real malloc once
        static auto real_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
        mallocCounter++;
        return real_malloc(size);
    }
}

TEST_CASE( "SingleFrequencySystemIntegration check peak", "[SingleFrequencySystemIntegration]" ) {

    constexpr float frequencyHz = 1000.0f;
    constexpr float spacingMeters = 0.05f;
    constexpr size_t M = 4;

    struct SingleFrequencyGeneratorConfig generatorConfig = {
        .snr = 40.0f,
        .thetaRad = M_PI / 4.0f,
        .frequency = frequencyHz
    };

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .singleFrequencySystemConfig = {
            .frequencyHz = frequencyHz,
            .nAngles = 360,
            .computeIntervalFrames = 5,
            .nSources = 1
        },
        .nAveragingFrames = 5,
        .spacingMeters = spacingMeters
    };

    UniformLinearArray<M> uniformLinearArray(spacingMeters);
    SingleFrequencyGenerator<M> signalGenerator(uniformLinearArray, generatorConfig);
    SingleFrequencySystemIntegration<M> system(systemConfig);

    std::array<std::complex<float>, M> inputFrame = signalGenerator.generateInput();

    //Print input frame
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == true);

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& pseudospectrum = system.getPseudospectrum();
    REQUIRE(pseudospectrum.size() == systemConfig.singleFrequencySystemConfig.nAngles);

    //Print pseudospectrum values
    size_t peakIndex = 0;
    float peakValue = 0.0f;
    for (size_t i = 0; i < pseudospectrum.size(); ++i) {
        if (pseudospectrum(i) > peakValue) {
            peakValue = pseudospectrum(i);
            peakIndex = i;
        }
    }
    float angleStepRad = (2.0f * M_PI) / static_cast<float>(systemConfig.singleFrequencySystemConfig.nAngles);
    float estimatedAngleRad = peakIndex * angleStepRad;

    REQUIRE(std::abs(estimatedAngleRad - generatorConfig.thetaRad) < angleStepRad);
}

TEST_CASE( "SingleFrequencySystemIntegration check memory alloc", "[SingleFrequencySystemIntegration]" ) {
    constexpr float frequencyHz = 1000.0f;
    constexpr float spacingMeters = 0.05f;
    constexpr size_t M = 4;

    struct SingleFrequencySystemIntegrationConfig systemConfig = {
        .singleFrequencySystemConfig = {
            .frequencyHz = frequencyHz,
            .nAngles = 360,
            .computeIntervalFrames = 5,
            .nSources = 1
        },
        .nAveragingFrames = 5,
        .spacingMeters = spacingMeters
    };

    UniformLinearArray<M> uniformLinearArray(spacingMeters);
    SingleFrequencySystemIntegration<M> system(systemConfig);
    std::array<std::complex<float>, M> inputFrame;

    size_t postInitMallocState = mallocCounter;
    std::cout << mallocCounter << std::endl;

    //Print input frame
    for (int i = 0; i < 5; i++) {
        system.processFrame(inputFrame);
    }

    REQUIRE(mallocCounter == postInitMallocState);
}
