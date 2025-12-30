#include "Eigen/Dense"
#include <complex>
#include <cmath>

template <size_t M>
class ISteeringVectorModel {
public:
    virtual ~ISteeringVectorModel() = default;

    virtual Eigen::Matrix<std::complex<float>, M, 1> getSteeringVector(float thetaRad, float frequencyHz) const = 0;
};
