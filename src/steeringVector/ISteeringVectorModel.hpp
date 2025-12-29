#include "Eigen/Dense"
#include <complex>
#include <cmath>

template <size_t N>
class ISteeringVectorModel {
public:
    virtual ~ISteeringVectorModel() = default;

    virtual Eigen::Matrix<std::complex<float>, N, 1> getSteeringVector(float thetaRad, float frequencyHz) const = 0;
};
