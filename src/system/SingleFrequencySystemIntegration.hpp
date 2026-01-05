#pragma once
#include "CircularBuffer.hpp"
#include "DspMusic.hpp"
#include "UniformLinearArray.hpp"
#include "SingleFrequencySystem.hpp"

struct SingleFrequencySystemIntegrationConfig {
    struct SingleFrequencySystemConfig singleFrequencySystemConfig;

    size_t nAveragingFrames;
    float  spacingMeters;
};

template <size_t M>
class SingleFrequencySystemIntegration {
private:
    static constexpr size_t nSourcesFixed = 1;

    CircularBuffer<M> circularBuffer_;
    DspMusic<M> dspMusic_;
    UniformLinearArray<M> uniformLinearArray_;
    SingleFrequencySystem<M> singleFrequencySystem_;

public:
    SingleFrequencySystemIntegration(struct SingleFrequencySystemIntegrationConfig config) :
        circularBuffer_(config.nAveragingFrames),
        dspMusic_(nSourcesFixed),
        uniformLinearArray_(config.spacingMeters),
        singleFrequencySystem_(config.singleFrequencySystemConfig, circularBuffer_, uniformLinearArray_, dspMusic_) {}

    bool processFrame(const std::array<std::complex<float>, M> &frame) {
        return singleFrequencySystem_.processFrame(frame);
    }

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& getPseudospectrum() const {
        return singleFrequencySystem_.getPseudospectrum();
    }
};
