#pragma once

// Common constants for MUSIC algorithm and steering vector precomputation

namespace MusicConstants {

// System configuration
constexpr float sampling_freq = 8000.0f;  // Sampling frequency in Hz
constexpr size_t fft_size = 256;          // FFT size

// Array configuration
constexpr size_t M = 4;  // Number of microphones
constexpr float spacing_meters = 0.05f;  // 5cm spacing
constexpr float speed_of_sound = 343.0f;  // Speed of sound in m/s

// MUSIC algorithm parameters
constexpr size_t n_angles = 256;  // Number of angles for pseudospectrum
constexpr size_t n_frequencies = 32;  // Number of frequencies to analyze

// Frequency range
constexpr float start_freq = 687.5f;  // Start frequency in Hz
constexpr float end_freq = 1687.5f;   // End frequency in Hz

// Derived constants
constexpr float freq_step = (end_freq - start_freq) / (n_frequencies - 1);
constexpr float angle_step = 2.0f * M_PI / n_angles;

} // namespace MusicConstants
