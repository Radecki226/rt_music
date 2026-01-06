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
    float calculatePseudospectrum(const Eigen::Matrix<std::complex<float>, M, 1> &steeringVector,
                                  const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace) override;
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
float DspMusic<M>::calculatePseudospectrum(const Eigen::Matrix<std::complex<float>, M, 1> &steeringVector,
                                           const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace) {

    if (noiseSpace.cols() != noiseSubspaceDim_) {
        throw std::invalid_argument("Noise space matrix has incorrect number of columns!");
    }

    noiseSpaceAdjont_ = noiseSpace.adjoint();

    projection_.noalias() = noiseSpaceAdjont_ * steeringVector;
    const float epsilon = 1e-9;
    float denom = projection_.squaredNorm() + epsilon;

    return 1.0f / denom;
}
