#pragma once
#include "ISteeringVectorModel.hpp"
#include <array>
#include <complex>
#include <random>

struct SingleFrequencyGeneratorConfig {
    float snr;
    float thetaRad;
    float frequency;
};

/**
 * @param M number of microphones.
 */
template<size_t M>
class SingleFrequencyGenerator {
private:
    SingleFrequencyGeneratorConfig config;
    ISteeringVectorModel<M> &device;
    
    std::mt19937 gen{std::random_device{}()};

public:
    SingleFrequencyGenerator(ISteeringVectorModel<M> &device, SingleFrequencyGeneratorConfig config) 
        : device(device), config(config) {}

    std::array<std::complex<float>, M> generateInput() {
        Eigen::Matrix<std::complex<float>, M, 1> sv = device.getSteeringVector(config.thetaRad, config.frequency);

        float snrLinear = std::pow(10.0f, config.snr / 10.0f);
        float noiseStdDev = std::sqrt(1.0f / (2.0f * snrLinear));

        std::normal_distribution<float> dist(0.0f, noiseStdDev);
        std::array<std::complex<float>, M> output;

        for (size_t i = 0; i < M; ++i) {
            std::complex<float> signal = sv(i);

            std::complex<float> noise(dist(gen), dist(gen));

            output[i] = signal + noise;
        }

        return output;
    }
};
