#pragma once

#include "Eigen/Dense"

template <size_t M>
class IDspMusic {
public:
    virtual ~IDspMusic() = default;

    virtual void computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                                   const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const = 0;
    
    virtual float calculatePseudospectrum(const Eigen::Matrix<std::complex<float>, M, 1> &steeringVector,
                                          const Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &noiseSpace) = 0;
};
