#pragma once
#include <cstddef>
#include "SingleFrequencySystemIntegration.hpp"

struct MultiFrequencySystemIntegrationConfig {
    float samplingFrequencyHz;
    float minFrequencyHz;
    float maxFrequencyHz;
    size_t fftSize;

    size_t nAveragingFrames;
    float  spacingMeters;
    
    size_t nAngles;
    size_t computeIntervalFrames;
    size_t nSources;
};

template <size_t M>
class MultiFrequencySystemIntegration {
private:
    std::vector<SingleFrequencySystemIntegration<M>> frequencySystems_;
    std::vector<size_t> frequencyIndices_;
public:
    MultiFrequencySystemIntegration(struct MultiFrequencySystemIntegrationConfig config) {
        for (size_t i = 0; i < config.fftSize; ++i) {
            float frequencyHz = config.samplingFrequencyHz / 2.0f * 
                                (static_cast<float>(i) / static_cast<float>(config.fftSize - 1));

            if (frequencyHz < config.minFrequencyHz || frequencyHz > config.maxFrequencyHz) {
                continue;
            }

            struct SingleFrequencySystemIntegrationConfig singleFreqConfig = {
                .singleFrequencySystemConfig = {
                    .frequencyHz = frequencyHz,
                    .nAngles = config.nAngles,
                    .computeIntervalFrames = config.computeIntervalFrames,
                    .nSources = config.nSources
                },
                .nAveragingFrames = config.nAveragingFrames,
                .spacingMeters = config.spacingMeters
            };

            frequencySystems_.emplace_back(singleFreqConfig);
            frequencyIndices_.emplace_back(i);
        }
    }

    bool processFrame(const std::vector<std::array<std::complex<float>, M>> &fftFrame) {
        bool anyComputed = false;
        for (size_t i = 0; i < frequencySystems_.size(); ++i) {
            bool computed = frequencySystems_[i].processFrame(fftFrame[frequencyIndices_[i]]);
            anyComputed = anyComputed || computed;
        }
        return anyComputed;
    }

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& getPseudospectrum() const {
        float sum = 0.0f;
        for (const auto& system : frequencySystems_) {
            sum += system.getPseudospectrum();
        }

        return sum / static_cast<float>(frequencySystems_.size());
    }
};
