#pragma once

#include <vector>
#include <array>
#include "Eigen/Dense"

template <size_t M>
class ICircularBuffer {
public:
    virtual ~ICircularBuffer() = default;

    /**
     * Push new chunk of data (M-len float array) into the circular buffer.
     * @param chunk Array of M float samples to push into the buffer.
     */
    virtual void push(const std::array<std::complex<float>, M> &column) = 0;

    /**
     * Calculate Covariance Matrix.
     */
    virtual void calcCov(Eigen::Matrix<std::complex<float>, M, M>& covMatrix) const = 0;
};
