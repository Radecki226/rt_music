#pragma once

#include <vector>
#include <array>
#include "Eigen/Dense"

template <size_t M>
class ICircularBuffer {
public:
    virtual ~ICircularBuffer() = default;

    virtual void push(const std::array<std::complex<float>, M> &column) = 0;

    virtual void calcCov(Eigen::Matrix<std::complex<float>, M, M>& covMatrix) const = 0;
};
