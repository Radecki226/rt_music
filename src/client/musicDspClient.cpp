#include <iostream>
#include <complex>
#include <array>
#include <music-dsp/musicDsp.hpp>

int main() {
    std::cout << "Initializing Music DSP Test App..." << std::endl;

    // Create a dummy frame for 2 microphones (nMic_ = 2)
    // In a real scenario, this data comes from your audio DMA/ADC
    std::array<std::complex<float>, 2> frame;
    frame[0] = {1.0f, 0.5f};
    frame[1] = {0.8f, 0.3f};

    for (int i = 0; i < 10000; i++) {
        bool success = musicDsp_processFrame(frame);
        
        if (success) {
            std::cout << "DSP Processing Successful!" << std::endl;
            
            // 2. Get the results
            const auto& spectrum = musicDsp_getPseudospectrum();
            std::cout << "Pseudospectrum size: " << spectrum.size() << std::endl;
            std::cout << "First 5 values: " << spectrum.head(5).transpose() << std::endl;
        } else {
            std::cerr << "DSP Processing failed (Wait for more frames?)" << std::endl;
        }
    }

    return 0;
}
