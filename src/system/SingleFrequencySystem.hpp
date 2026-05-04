#pragma once
#include <cstddef>
#include "ICircularBuffer.hpp"
#include "steering_vectors.hpp"
#include "IDspMusic.hpp"
#include <optional>
#include <vector>
#include <complex>
#include "Eigen/Dense"
#include <iostream>

struct SingleFrequencySystemConfig {
    size_t frequencyIdx;

    size_t nAngles;

    size_t computeIntervalFrames;

    size_t nSources;
};

template <size_t M>
class SingleFrequencySystem {
private:
    SingleFrequencySystemConfig config_;

    ICircularBuffer<M> &circularBuffer_;
    IDspMusic<M> &dspMusic_;

    Eigen::Matrix<std::complex<float>, M, M> covMatrix_;
    Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> noiseSpace_;
    Eigen::Matrix<float, Eigen::Dynamic, 1> pseudospectrum_;
    Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> steeringVectors_;

    size_t frameCounter_ = 0;

public:
    explicit SingleFrequencySystem(const SingleFrequencySystemConfig &config, 
                                   ICircularBuffer<M> &circularBuffer,
                                   IDspMusic<M> &dspMusic) : config_(config),
                                   circularBuffer_(circularBuffer), 
                                   dspMusic_(dspMusic) {
        covMatrix_.setZero();

        noiseSpace_.resize(M, M - config.nSources);
        noiseSpace_.setZero();

        pseudospectrum_.resize(config.nAngles);
        pseudospectrum_.setZero();

        steeringVectors_ = steering_vectors[config_.frequencyIdx].transpose();
    }

    bool processFrame(const std::array<std::complex<float>, M> &frame) {
        circularBuffer_.push(frame);

        frameCounter_++;
        if (frameCounter_ < config_.computeIntervalFrames) {
            return false;
        }
    
        frameCounter_ = 0;
        circularBuffer_.calcCov(covMatrix_);
        dspMusic_.computeNoiseSpace(noiseSpace_, covMatrix_);

        dspMusic_.calculatePseudospectrumBatch(steeringVectors_, noiseSpace_, pseudospectrum_);

        return true;
    }

    const Eigen::Matrix<float, Eigen::Dynamic, 1>& getPseudospectrum() const {
        return pseudospectrum_;
    }
};

