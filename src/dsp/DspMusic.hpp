#pragma once
#include "Eigen/Dense"
#include "IDspMusic.hpp"
#include "ICircularBuffer.hpp"
#include <stdexcept>

template <size_t M>
class DspMusic : public IDspMusic<M> {
private:
    static_assert(
        M > 0, 
        "Error: Number of microphones can't be 0!"
    );

    size_t noiseSubspaceDim_{};
    size_t nAngles_{};

    Eigen::Matrix<std::complex<float>, Eigen::Dynamic, M> noiseSpaceAdjont_;
    Eigen::Matrix<std::complex<float>, Eigen::Dynamic, Eigen::Dynamic> projections_;

public:
    /**
     * @param signalSubspaceDim Effectively number of sources to be estimated.
     * @param nAngles Number of steering-vector columns (angle grid size); buffers are sized here.
     */
    explicit DspMusic(size_t signalSubspaceDim, size_t nAngles) {
        reconfig(signalSubspaceDim, nAngles);
    }

    void reconfig(size_t newSignalSubspaceDim, size_t nAngles) {
        if (newSignalSubspaceDim == 0) {
            throw std::invalid_argument("Signal Subspace Dimension must be larger than 0!");
        }

        if (newSignalSubspaceDim >= M) {
            throw std::invalid_argument("Signal Subspace Dimension must be less than number of microphones!");
        }

        if (nAngles == 0) {
            throw std::invalid_argument("nAngles must be larger than 0!");
        }

        noiseSubspaceDim_ = M - newSignalSubspaceDim;
        nAngles_ = nAngles;

        noiseSpaceAdjont_.resize(noiseSubspaceDim_, M);
        noiseSpaceAdjont_.setZero();

        projections_.resize(noiseSubspaceDim_, nAngles_);
        projections_.setZero();
    }

    void computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                           const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const override;
    void calculatePseudospectrumBatch(
        const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &steeringVectors,
        const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace,
        Eigen::Ref<Eigen::Matrix<float, Eigen::Dynamic, 1>> pseudospectrumOut) override;
};

template <size_t M>
void DspMusic<M>::computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                                    const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const {

    if (output.cols() != noiseSubspaceDim_) {
       throw std::invalid_argument("Output matrix has incorrect number of columns!");
    }

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix<std::complex<float>, M, M>> eigenSolver(covMatrix);

    //TODO: implement some fallback
    if (eigenSolver.info() != Eigen::Success) {
        throw std::runtime_error("Eigen decomposition failed!");
    }

    for (size_t i = 0; i < noiseSubspaceDim_; ++i) {
        output.col(i) = eigenSolver.eigenvectors().col(i);
    }
}

template <size_t M>
void DspMusic<M>::calculatePseudospectrumBatch(
    const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &steeringVectors,
    const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace,
    Eigen::Ref<Eigen::Matrix<float, Eigen::Dynamic, 1>> pseudospectrumOut) {

    if (noiseSpace.cols() != noiseSubspaceDim_) {
        throw std::invalid_argument("Noise space matrix has incorrect number of columns!");
    }

    if (static_cast<size_t>(steeringVectors.cols()) != nAngles_) {
        throw std::invalid_argument("Steering vector column count does not match configured nAngles!");
    }

    if (pseudospectrumOut.size() != static_cast<Eigen::Index>(nAngles_)) {
        throw std::invalid_argument("Output pseudospectrum length does not match configured nAngles!");
    }

    noiseSpaceAdjont_ = noiseSpace.adjoint();
    projections_.noalias() = noiseSpaceAdjont_ * steeringVectors;

    const float epsilon = 1e-9;

    for (size_t i = 0; i < nAngles_; ++i) {
        const float denom = projections_.col(static_cast<Eigen::Index>(i)).squaredNorm() + epsilon;
        pseudospectrumOut(static_cast<Eigen::Index>(i)) = 1.0f / denom;
    }
}
