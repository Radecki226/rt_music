#include <complex>
#include "Eigen/Dense"
#include <cstddef>
#include <array>

size_t constexpr musicDsp_nMic = 2;

bool musicDsp_processFrame(const std::array<std::complex<float>, musicDsp_nMic> &frame);

const Eigen::Matrix<float, Eigen::Dynamic, 1>& musicDsp_getPseudospectrum();
