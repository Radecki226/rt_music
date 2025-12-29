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

    size_t signalSubspaceDim_;

public:
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

        signalSubspaceDim_ = newSignalSubspaceDim;
    }

    void computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                           const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const override;
};

template <size_t M>
void DspMusic<M>::computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                                    const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const {

    size_t noiseSubspaceDim = M - signalSubspaceDim_;

    if (output.cols() != noiseSubspaceDim) {
       throw std::invalid_argument("Output matrix has incorrect number of columns!");
    }

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix<std::complex<float>, M, M>> eigenSolver(covMatrix);

    //TODO: implement some fallback
    if (eigenSolver.info() != Eigen::Success) {
        throw std::runtime_error("Eigen decomposition failed!");
    }

    for (size_t i = 0; i < noiseSubspaceDim; ++i) {
        output.col(i) = eigenSolver.eigenvectors().col(i);
    }
}
