#include "musicDsp.hpp"
#include "SingleFrequencySystemIntegration.hpp"

static SingleFrequencySystemIntegrationConfig config_  = {
    .singleFrequencySystemConfig = {
        .frequencyHz = 1000,
        .nAngles = 360,
        .computeIntervalFrames = 10,
        .nSources = 1
    },
    .nAveragingFrames = 10,
    .spacingMeters = 0.05f
};

static SingleFrequencySystemIntegration<musicDsp_nMic> integration_(config_);

bool musicDsp_processFrame(const std::array<std::complex<float>, musicDsp_nMic> &frame) {
    return integration_.processFrame(frame);
}

const Eigen::Matrix<float, Eigen::Dynamic, 1>& musicDsp_getPseudospectrum() {
    return integration_.getPseudospectrum();
}
