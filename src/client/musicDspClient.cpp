#include <iostream>
#include <complex>
#include <array>
#include <music-dsp/musicDsp.hpp>

int main() {
    std::cout << "Initializing Music DSP Test App..." << std::endl;

    std::array<std::complex<float>, musicDsp_nMic> frame{};
    frame[0] = {1.0f, 0.5f};
    frame[1] = {0.8f, 0.3f};
    frame[2] = {0.2f, 0.1f};
    frame[3] = {-0.1f, 0.4f};

    for (int i = 0; i < 10000; i++) {
        bool success = musicDsp_processFrame(frame);
        
        if (success) {
            std::cout << "DSP Processing Successful!" << std::endl;

            const auto& spectrum = musicDsp_getPseudospectrum();
            std::cout << "Pseudospectrum size: " << spectrum.size() << std::endl;
            std::cout << "First 5 values: " << spectrum.head(5).transpose() << std::endl;
        } else {
            std::cerr << "DSP Processing failed (Wait for more frames?)" << std::endl;
        }
    }

    return 0;
}
