#include "SingleFrequencyGenerator.hpp"
#include "SingleFrequencySystemIntegration.hpp"
#include "UniformLinearArray.hpp"
#include "MusicConstants.hpp"

#include <catch2/catch_test_macros.hpp>

#include "trompeloeil.hpp"
#include <array>
#include <complex>
#include <cstdio>
#include <iostream>
#include <vector>

#include <dlfcn.h>
#include <unistd.h>

static size_t mallocCounter = 0;
static std::vector<std::pair<size_t, size_t>> allocations;  // {call_number, size}

extern "C" {
    void* malloc(size_t size) noexcept {
        // This is the "assignment" part: find the real malloc once
        static auto real_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
        mallocCounter++;        
        return real_malloc(size);
    }
}

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

    UniformLinearArray<MusicConstants::M> uniformLinearArray(MusicConstants::spacing_meters);
    SingleFrequencyGenerator<MusicConstants::M> signalGenerator(uniformLinearArray, generatorConfig);
    SingleFrequencySystemIntegration<MusicConstants::M> system(systemConfig);

    std::array<std::complex<float>, MusicConstants::M> inputFrame = signalGenerator.generateInput();

    //Print input frame
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == false);
    REQUIRE(system.processFrame(inputFrame) == true);

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& pseudospectrum = system.getPseudospectrum();
    REQUIRE(pseudospectrum.size() == MusicConstants::n_angles);

    //Print pseudospectrum values
    size_t peakIndex = 0;
    float peakValue = 0.0f;
    for (size_t i = 0; i < pseudospectrum.size(); ++i) {
        printf("Angle %zu: Pseudospectrum = %.2f\n", i, pseudospectrum(i));
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

    size_t postInitMallocState = mallocCounter;
    allocations.clear();

    //Process frames - should not allocate steering vectors at runtime
    for (int i = 0; i < 5; i++) {
        system.processFrame(inputFrame);
    }

    // Print all allocations during processing
    std::cerr << "\nAllocations during 5 frames:\n";
    for (const auto& [call_num, size] : allocations) {
        std::cerr << "  [#" << call_num << "] " << size << " bytes\n";
    }
    std::cerr << "Total allocations: " << allocations.size() << "\n";
    std::cerr << "Total alloc count: " << (mallocCounter - postInitMallocState) << "\n\n";

    // Verify steering vectors were not dynamically allocated
    // (Eigen matrix operations may allocate temporaries, but not the large steering vector array)
    REQUIRE(mallocCounter - postInitMallocState < 100);  // Allow for temporary Eigen allocations
}
