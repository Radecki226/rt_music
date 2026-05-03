#include <complex>
#include "Eigen/Dense"
#include "MusicConstants.hpp"
#include <cstddef>
#include <array>

size_t constexpr musicDsp_nMic = MusicConstants::M;

bool musicDsp_processFrame(const std::array<std::complex<float>, musicDsp_nMic> &frame);

const Eigen::Matrix<float, Eigen::Dynamic, 1>& musicDsp_getPseudospectrum();
