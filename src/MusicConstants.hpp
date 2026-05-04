#pragma once

#include <cmath>

namespace MusicConstants {

constexpr float sampling_freq = 8000.0f;
constexpr size_t fft_size = 256;

constexpr size_t M = 4;
constexpr float spacing_meters = 0.05f;
constexpr float speed_of_sound = 343.0f;

constexpr size_t n_angles = 256;
constexpr size_t n_frequencies = 32;

constexpr float start_freq = 687.5f;
constexpr float end_freq = 1687.5f;

/** Grid uses [start_freq, end_freq); last bin is start + (n_frequencies - 1) * freq_step. */
constexpr float freq_step =
    (end_freq - start_freq) / static_cast<float>(n_frequencies);
constexpr float angle_step = 2.0f * M_PI / n_angles;

}
