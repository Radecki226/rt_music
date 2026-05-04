#pragma once
#include "CircularBuffer.hpp"
#include "DspMusic.hpp"
#include "SingleFrequencySystem.hpp"
#include "MusicConstants.hpp"

struct SingleFrequencySystemIntegrationConfig {
    size_t computeIntervalFrames;
    size_t nSources;
    size_t nAveragingFrames;
};

template <size_t M>
class SingleFrequencySystemIntegration {
private:
    CircularBuffer<M> circularBuffer_;
    DspMusic<M> dspMusic_;
    SingleFrequencySystem<M> singleFrequencySystem_;

public:
    SingleFrequencySystemIntegration(struct SingleFrequencySystemIntegrationConfig config) :
        circularBuffer_(config.nAveragingFrames),
        dspMusic_(config.nSources, MusicConstants::n_angles),
        singleFrequencySystem_({
            .frequencyIdx = 15, //Fixed on 1187.5 Hz
            .nAngles = MusicConstants::n_angles,
            .computeIntervalFrames = config.computeIntervalFrames,
            .nSources = config.nSources
        }, circularBuffer_, dspMusic_) {
    }

    bool processFrame(const std::array<std::complex<float>, M> &frame) {
        return singleFrequencySystem_.processFrame(frame);
    }

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& getPseudospectrum() const {
        return singleFrequencySystem_.getPseudospectrum();
    }
};
