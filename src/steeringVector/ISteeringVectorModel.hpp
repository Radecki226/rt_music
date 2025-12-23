#include "Eigen/Dense"
#include <complex>
#include <cmath>

template <int N>
class ISteeringVectorModel {
public:
    virtual ~ISteeringVectorModel() = default;

    virtual Eigen::Matrix<std::complex<float>, N, 1> getSteeringVector(float theta_rad, float frequency_hz) const = 0;
};
