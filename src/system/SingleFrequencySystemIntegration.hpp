#pragma once
#include "CircularBuffer.hpp"
#include "DspMusic.hpp"
#include "UniformLinearArray.hpp"
#include "SingleFrequencySystem.hpp"

struct SingleFrequencySystemIntegrationConfig {
    struct SingleFrequencySystemConfig singleFrequencySystemConfig;

    size_t nAveragingFrames;
};

template <size_t M>
class SingleFrequencySystemIntegration {
private:
    static constexpr size_t nSourcesFixed = 1;
    static constexpr float spacingFixed = 0.05;

    CircularBuffer<M> circularBuffer_;
    DspMusic<M> dspMusic_;
    UniformLinearArray<M> uniformLinearArray_;
    SingleFrequencySystem<M> singleFrequencySystem_;

public:
    SingleFrequencySystemIntegration(struct SingleFrequencySystemIntegrationConfig config) {
        circularBuffer_(config.nAveragingFrames);
        dspMusic_(nSourcesFixed);
        uniformLinearArray_(spacingFixed);
        singleFrequencySystem(config, circularBuffer_, dspMusic_, uniformLinearArray_);
    }
};
