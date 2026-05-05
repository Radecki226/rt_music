#pragma once
#include "ISteeringVectorModel.hpp"
#include <array>
#include <complex>
#include <random>
#include <fstream>
#include <cstdint>
#include "kiss_fft.h"
#include "MusicConstants.hpp"

/**
 * @brief This class is supposed to read a file and generate multisensor spectrogram.
 * The file is supposed to be a following format:
 * One line = One sample in float format
 * 
 * Generator is quite naive. It just partitiones data into FFT-size blocks without appying any window or overlap.
 */

struct FileBasedGeneratorConfig {
    std::string filename;
    float thetaRad;
};

template<size_t M>
class FileBasedGenerator {
private:
    /* data[frame][frequency][microphone] */
    
    std::vector<std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies>> data;

public:
    FileBasedGenerator(ISteeringVectorModel<M> &device, FileBasedGeneratorConfig config) 
        {
            std::ifstream file(config.filename, std::ios::binary);

            std::vector<float> samples;

            //Read line by line
            std::string line;
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                float sample;
                while (ss >> sample) {
                    samples.push_back(sample);
                }
            }

            //Compute FFT
            kiss_fft_cfg cfg = kiss_fft_alloc(MusicConstants::fft_size, 0, 0, 0);
            kiss_fft_cpx input [MusicConstants::fft_size];
            kiss_fft_cpx output [MusicConstants::fft_size];

            std::complex<float> outputComplex[MusicConstants::fft_size];

            for (size_t i = 0; i < samples.size(); i += MusicConstants::fft_size) {
                for (size_t j = 0; j < MusicConstants::fft_size; j++) {
                    input[j].r = samples[i + j];
                    input[j].i = 0;
                }

                kiss_fft(cfg, input, output);

                for (size_t j = 0; j < MusicConstants::fft_size; j++) {
                    outputComplex[j] = std::complex<float>(output[j].r, output[j].i);
                }
    
                //Use steering vector to compute spectrogram for each of M microphones
                std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies> frame;
                for (int f = 0; f < MusicConstants::n_frequencies; f++) {
                    size_t global_frequency_offset = MusicConstants::start_freq_index + f;
                    const float frequency =
                        MusicConstants::sampling_freq * static_cast<float>(global_frequency_offset)
                        / static_cast<float>(MusicConstants::fft_size);

                    Eigen::Matrix<std::complex<float>, M, 1> steeringVector = device.getSteeringVector(config.thetaRad, frequency);
    
                    for (int m = 0; m < MusicConstants::M; m++) {
                        frame[f][m] = outputComplex[global_frequency_offset] * steeringVector(m);
                    }
                }

                data.push_back(frame);
            }

            kiss_fft_free(cfg);
        }
    
    std::vector<std::array<std::array<std::complex<float>, MusicConstants::M>, MusicConstants::n_frequencies>> getData() const {
        return data;
    }
};
