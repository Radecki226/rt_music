#pragma once
#include <cstddef>
#include "ICircularBuffer.hpp"
#include "ISteeringVectorModel.hpp"
#include "IDspMusic.hpp"
#include <optional>
#include <vector>
#include <complex>
#include "Eigen/Dense"
#include <iostream>

struct SingleFrequencySystemConfig {
    /*Frequency to be analyzed in Hz*/
    float frequencyHz;

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
    ISteeringVectorModel<M>& steeringVectorModel_;
    IDspMusic<M> &dspMusic_;

    Eigen::Matrix<std::complex<float>, M, M> covMatrix_;
    Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> noiseSpace_;
    Eigen::Matrix<float, Eigen::Dynamic, 1> pseudospectrum_;

    //TODO: Potentially optimize to use vector ops
    std::vector<float> thetaAngles_;

    size_t frameCounter_ = 0;

public:
    explicit SingleFrequencySystem(const SingleFrequencySystemConfig &config, 
                                   ICircularBuffer<M> &circularBuffer,
                                   ISteeringVectorModel<M> &steeringVectorModel, 
                                   IDspMusic<M> &dspMusic) : config_(config),
                                   circularBuffer_(circularBuffer), 
                                   steeringVectorModel_(steeringVectorModel), dspMusic_(dspMusic) {
        covMatrix_.setZero();

        noiseSpace_.resize(M, M - config.nSources);
        noiseSpace_.setZero();

        pseudospectrum_.resize(config.nAngles);
        pseudospectrum_.setZero();

        float angleStepRad = (2.0f * M_PI) / static_cast<float>(config.nAngles);
        for (size_t i = 0; i < config.nAngles; ++i) {
            float theta = i * angleStepRad;
            thetaAngles_.push_back(theta);
        }
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

        for (size_t i = 0; i < config_.nAngles; ++i) {
            Eigen::Matrix<std::complex<float>, M, 1> steeringVector =
                steeringVectorModel_.getSteeringVector(thetaAngles_[i], config_.frequencyHz);
            float psValue = dspMusic_.calculatePseudospectrum(steeringVector, noiseSpace_);
            pseudospectrum_(i) = psValue;
        }
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

