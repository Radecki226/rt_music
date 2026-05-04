#pragma once

#include <array>
#include <complex>
#include <cstddef>
#include <vector>

#include "CircularBuffer.hpp"
#include "DspMusic.hpp"
#include "Eigen/Dense"
#include "MusicConstants.hpp"
#include "SingleFrequencySystem.hpp"

struct MultiFrequencySystemIntegrationConfig {
    size_t computeIntervalFrames;
    size_t nSources;
    size_t nAveragingFrames;
};

template <size_t M>
class MultiFrequencySystemIntegration {
private:
    std::vector<CircularBuffer<M>> circularBuffers_;
    DspMusic<M> dspMusic_;
    std::vector<SingleFrequencySystem<M>> singleFrequencySystems_;
    mutable Eigen::Matrix<float, Eigen::Dynamic, 1> aggregatedPseudospectrum_;

public:
    explicit MultiFrequencySystemIntegration(struct MultiFrequencySystemIntegrationConfig config)
        : dspMusic_(config.nSources, MusicConstants::n_angles) {
        circularBuffers_.reserve(MusicConstants::n_frequencies);
        singleFrequencySystems_.reserve(MusicConstants::n_frequencies);

        for (size_t i = 0; i < MusicConstants::n_frequencies; ++i) {
            circularBuffers_.emplace_back(config.nAveragingFrames);
        }

        for (size_t i = 0; i < MusicConstants::n_frequencies; ++i) {
            singleFrequencySystems_.emplace_back(
                SingleFrequencySystemConfig{
                    .frequencyIdx = i,
                    .nAngles = MusicConstants::n_angles,
                    .computeIntervalFrames = config.computeIntervalFrames,
                    .nSources = config.nSources,
                },
                circularBuffers_[i],
                dspMusic_);
        }

        aggregatedPseudospectrum_.resize(static_cast<Eigen::Index>(MusicConstants::n_angles));
        aggregatedPseudospectrum_.setZero();
    }

    bool processFrame(
        const std::array<std::array<std::complex<float>, M>, MusicConstants::n_frequencies> &perFrequency) {
        bool anyComputed = false;
        for (size_t i = 0; i < MusicConstants::n_frequencies; ++i) {
            anyComputed |= singleFrequencySystems_[i].processFrame(perFrequency[i]);
        }
        return anyComputed;
    }

    const Eigen::Matrix<float, Eigen::Dynamic, 1> &getPseudospectrum() const {
        aggregatedPseudospectrum_.setZero();
        for (size_t i = 0; i < singleFrequencySystems_.size(); ++i) {
            aggregatedPseudospectrum_ += singleFrequencySystems_[i].getPseudospectrum();
        }
        aggregatedPseudospectrum_ *= 1.0f / static_cast<float>(singleFrequencySystems_.size());
        return aggregatedPseudospectrum_;
    }
};
