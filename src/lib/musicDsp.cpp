#include "musicDsp.hpp"
#include "MusicConstants.hpp"
#include "SingleFrequencySystemIntegration.hpp"

static SingleFrequencySystemIntegrationConfig config_ = {
    .computeIntervalFrames = 10,
    .nSources = 1,
    .nAveragingFrames = 10,
};

static SingleFrequencySystemIntegration<MusicConstants::M> integration_(config_);

bool musicDsp_processFrame(const std::array<std::complex<float>, musicDsp_nMic> &frame) {
    return integration_.processFrame(frame);
}

const Eigen::Matrix<float, Eigen::Dynamic, 1>& musicDsp_getPseudospectrum() {
    return integration_.getPseudospectrum();
}
