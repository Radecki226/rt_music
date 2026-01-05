#pragma once
#include "ISteeringVectorModel.hpp"

template <size_t M>
class UniformLinearArray : public ISteeringVectorModel<M> {
public:
    explicit UniformLinearArray(float d_meters, float c = 343.0) 
        : d(d_meters), speedOfSound(c) {}

    Eigen::Matrix<std::complex<float>, M, 1> getSteeringVector(float theta_rad, float frequency_hz) const override {
        Eigen::Matrix<std::complex<float>, M, 1> sv;

        float lambda = speedOfSound / frequency_hz;
        float k = 2.0 * M_PI / lambda;
        
        // Phase shift between adjacent elements: delta_phi = k * d * sin(theta)
        // We use sin(theta) assuming theta=0 is boresight (perpendicular to array)
        std::complex<float> phase_step = std::exp(std::complex<float>(0, k * d * std::sin(theta_rad)));

        sv[0] = std::complex<float>(1.0, 0.0);
        for (int i = 1; i < M; ++i) {
            // Recurrence relation: avoids multiple calls to sin() and exp()
            sv[i] = sv[i - 1] * phase_step;
        }

        return sv;
    }

private:
    float d;
    float speedOfSound;
};
