#pragma once
#include "Eigen/Dense"
#include "IDspMusic.hpp"
#include "ICircularBuffer.hpp"

template <size_t M>
class DspMusic : public IDspMusic<M> {
private:
    static_assert(
        M > 0, 
        "Error: Number of microphones can't be 0!"
    );

    size_t noiseSubspaceDim_;

    Eigen::Matrix<std::complex<float>, Eigen::Dynamic, M> noiseSpaceAdjont_;
    Eigen::Matrix<std::complex<float>, Eigen::Dynamic, 1> projection_;
    Eigen::Matrix<std::complex<float>, Eigen::Dynamic, Eigen::Dynamic> projections_;

public:
    /**
     * Constructor for DspMusic class.
     * @param signalSubspaceDim Effectively number of sources to be estimated.
     */
    explicit DspMusic(size_t signalSubspaceDim) {
        reconfig(signalSubspaceDim);
    }

    void reconfig(size_t newSignalSubspaceDim) {
        if (newSignalSubspaceDim == 0) {
            throw std::invalid_argument("Signal Subspace Dimension must be larger than 0!");
        }

        if (newSignalSubspaceDim >= M) {
            throw std::invalid_argument("Signal Subspace Dimension must be less than number of microphones!");
        }

        noiseSubspaceDim_ = M - newSignalSubspaceDim;

        noiseSpaceAdjont_.resize(noiseSubspaceDim_, M);
        noiseSpaceAdjont_.setZero();

        projection_.resize(noiseSubspaceDim_, 1);
        projection_.setZero();
    }

    void computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                           const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const override;
    Eigen::Matrix<float, Eigen::Dynamic, 1> calculatePseudospectrumBatch(
                                  const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &steeringVectors,
                                  const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace);
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
Eigen::Matrix<float, Eigen::Dynamic, 1> DspMusic<M>::calculatePseudospectrumBatch(
                                  const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &steeringVectors,
                                  const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace) {

    if (noiseSpace.cols() != noiseSubspaceDim_) {
        throw std::invalid_argument("Noise space matrix has incorrect number of columns!");
    }

    size_t nAngles = steeringVectors.cols();
    
    noiseSpaceAdjont_ = noiseSpace.adjoint();
    projections_.noalias() = noiseSpaceAdjont_ * steeringVectors;
    
    Eigen::Matrix<float, Eigen::Dynamic, 1> pseudospectrum(nAngles);
    const float epsilon = 1e-9;
    
    for (size_t i = 0; i < nAngles; ++i) {
        float denom = projections_.col(i).squaredNorm() + epsilon;
        pseudospectrum(i) = 1.0f / denom;
    }

    return pseudospectrum;
}
