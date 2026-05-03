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
    /*Frequency to be analyzed in Hz*/
    size_t frequencyIdx;

    /*Angle resolution in radians for the MUSIC spectrum*/
    size_t nAngles;

    /*How often music should be calculated, in frames. N means that every N frames*/
    size_t computeIntervalFrames;

    /*Number of signal sources to estimate*/
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

    /**
     * Process a new frame of data.
     * @param frame New frame of data to process.
     * @return True if MUSIC spectrum was computed, false otherwise.
     */
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

    /**
     * Get the last computed MUSIC pseudospectrum.
     * @return Reference to the pseudospectrum matrix.
     */
    const Eigen::Matrix<float, Eigen::Dynamic, 1>& getPseudospectrum() const {
        return pseudospectrum_;
    }
};

