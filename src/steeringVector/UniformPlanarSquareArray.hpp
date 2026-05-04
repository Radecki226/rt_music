#pragma once
#include "ISteeringVectorModel.hpp"

namespace steering_vector_detail {

template <std::size_t M>
constexpr std::size_t isqrt_floor() noexcept {
    std::size_t n = 0;
    while ((n + 1u) * (n + 1u) <= M) {
        ++n;
    }
    return n;
}

template <std::size_t N>
constexpr bool is_power_of_two() noexcept {
    return N > 0u && ((N & (N - 1u)) == 0u);
}

}

template <std::size_t M>
class UniformPlanarSquareArray : public ISteeringVectorModel<M> {
    static constexpr std::size_t N = steering_vector_detail::isqrt_floor<M>();
    static_assert(N * N == M,
                  "UniformPlanarSquareArray: M must be a perfect square (N×N microphones)");
    static_assert(steering_vector_detail::is_power_of_two<N>(),
                  "UniformPlanarSquareArray: grid side N = sqrt(M) must be a power of two");

public:
    explicit UniformPlanarSquareArray(float d_meters, float c = 343.0f)
        : d(d_meters), speedOfSound(c) {}

    Eigen::Matrix<std::complex<float>, M, 1> getSteeringVector(float theta_rad,
                                                               float frequency_hz) const override {
        Eigen::Matrix<std::complex<float>, M, 1> sv;

        const float lambda = speedOfSound / frequency_hz;
        const float k = 2.0f * static_cast<float>(M_PI) / lambda;
        const float cth = std::cos(theta_rad);
        const float sth = std::sin(theta_rad);

        std::size_t idx = 0;
        for (std::size_t j = 0; j < N; ++j) {
            for (std::size_t i = 0; i < N; ++i) {
                const float phase = k * d * (static_cast<float>(i) * cth + static_cast<float>(j) * sth);
                sv(static_cast<Eigen::Index>(idx), 0) = std::exp(std::complex<float>(0.0f, phase));
                ++idx;
            }
        }
        return sv;
    }

private:
    float d;
    float speedOfSound;
};
