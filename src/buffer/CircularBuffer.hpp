#pragma once

#include "ICircularBuffer.hpp"

#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <mutex>
#include <array>
#include <complex>
#include "Eigen/Dense"

/**
 * Implementation of Circular Buffer.
 * For the time being covariance matrix doesn't perform mean before calculation.
 * Data is supposed to have mean ~0.
 */

template <size_t M>
class CircularBuffer : public ICircularBuffer<M> {
private:
    static_assert(
        M > 0, 
        "Error: Number of microphones can't be 0!"
    );

    Eigen::Matrix<std::complex<float>, M, Eigen::Dynamic> matrix_;

    size_t headColumn_ = 0;

    mutable std::mutex accessMutex_;

public:
    explicit CircularBuffer(size_t initialColumns);

    void reconfig(size_t newColumns);

    void push(const std::array<std::complex<float>, M> &column) override;

    void calcCov(Eigen::Matrix<std::complex<float>, M, M>&) const override;
};

template <size_t M>
CircularBuffer<M>::CircularBuffer(size_t initialColumns) {
    reconfig(initialColumns);
}

template <size_t M>
void CircularBuffer<M>::reconfig(size_t newColumns) {
    if (newColumns == 0) {
        throw std::invalid_argument("N columns must be larger than 0!");
    }

    std::lock_guard<std::mutex> lock(accessMutex_);

    matrix_.resize(M, newColumns);
    matrix_.setZero();

    headColumn_ = 0;
}

template <size_t M>
void CircularBuffer<M>::push(const std::array<std::complex<float>, M> &column) {
    std::lock_guard<std::mutex> lock(accessMutex_); 

    Eigen::Map<const Eigen::Matrix<std::complex<float>, M, 1>> columnView(column.data(), column.size());
    matrix_.col(headColumn_) = columnView;

    headColumn_ = (headColumn_ + 1) % matrix_.cols();
}

template <size_t M>
void CircularBuffer<M>::calcCov(Eigen::Matrix<std::complex<float>, M, M> &cov) const {
    cov = matrix_ * matrix_.adjoint() / matrix_.cols();
}
