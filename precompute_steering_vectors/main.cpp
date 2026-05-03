#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <array>
#include "UniformLinearArray.hpp"
#include "../src/MusicConstants.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    using namespace MusicConstants;

    // Calculate frequency step
    const float freq_step = (end_freq - start_freq) / (n_frequencies - 1);

    // Create output directory
    fs::path output_dir = "steering_vectors";
    fs::create_directories(output_dir);

    // Create steering vector model
    UniformLinearArray<M> ula(spacing_meters, speed_of_sound);

    // Generate angles: 0 to 2π
    std::vector<float> angles(n_angles);
    const float angle_step = 2.0f * M_PI / n_angles;
    for (size_t i = 0; i < n_angles; ++i) {
        angles[i] = i * angle_step;
    }

    std::cout << "Precomputing steering vectors for " << n_frequencies
              << " frequencies and " << n_angles << " angles..." << std::endl;

        // Create filename
        char filename[256];
        std::snprintf(filename, sizeof(filename), "steering_vectors/steering_vectors.hpp");

        // Write header file
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }

        // Write header guard and includes
        file << "#pragma once\n";
        file << "#include <Eigen/Dense>\n";
        file << "#include <complex>\n";
        file << "#include <array>\n\n";

        // Write array of matrices
        file << "// Array of steering vector matrices for all frequencies\n";
        file << "static const std::array<Eigen::Matrix<std::complex<float>, " << n_angles << ", " << M << ">, " << n_frequencies << "> steering_vectors = {\n";

        for (size_t freq_idx = 0; freq_idx < n_frequencies; ++freq_idx) {
            const float frequency = start_freq + freq_idx * freq_step;

            // Create steering vector matrix for all angles (n_angles x M)
            Eigen::Matrix<std::complex<float>, Eigen::Dynamic, Eigen::Dynamic> steering_matrix(n_angles, M);

            for (size_t angle_idx = 0; angle_idx < n_angles; ++angle_idx) {
                auto steering_vec = ula.getSteeringVector(angles[angle_idx], frequency);
                for (size_t mic_idx = 0; mic_idx < M; ++mic_idx) {
                    steering_matrix(angle_idx, mic_idx) = steering_vec(mic_idx);
                }
            }

            file << "    Eigen::Matrix<std::complex<float>, " << n_angles << ", " << M << ">{\n";
            file << "        {\n";
            for (size_t angle_idx = 0; angle_idx < n_angles; ++angle_idx) {
                file << "            {";
                for (size_t mic_idx = 0; mic_idx < M; ++mic_idx) {
                    const auto& val = steering_matrix(angle_idx, mic_idx);
                    file << "std::complex<float>(" << std::fixed << std::setprecision(6) << val.real() << "f, " << val.imag() << "f)";
                    if (mic_idx < M - 1) {
                        file << ", ";
                    }
                }
                file << "}";
                if (angle_idx < n_angles - 1) {
                    file << ",";
                }
                file << "\n";
            }
            file << "        }\n";
            file << "    }";
            if (freq_idx < n_frequencies - 1) {
                file << ",";
            }
            file << "\n";

            std::cout << "Processed frequency " << frequency << " Hz" << std::endl;
        }

        file << "};\n";

        file.close();
        std::cout << "Generated: " << filename << std::endl;

    std::cout << "Precomputation complete! Files saved in '" << output_dir.string() << "' directory." << std::endl;
    return 0;
}