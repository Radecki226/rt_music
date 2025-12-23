#pragma once

#include "Eigen/Dense"

template <size_t M>
class IDspMusic {
public:
    virtual ~IDspMusic() = default;

    /**
     * Calculate Covariance Matrix.
     */
    virtual void computeNoiseSpace(Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> &output,
                                   const Eigen::Matrix<std::complex<float>, M, M> &covMatrix) const = 0;
};
