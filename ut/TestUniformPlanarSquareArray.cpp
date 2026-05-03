#include <catch2/catch_test_macros.hpp>
#include "UniformPlanarSquareArray.hpp"
#include <complex>

namespace {

bool inTolerance(const std::complex<float>& a, const std::complex<float>& b, float tol) {
    return (std::abs(a.real() - b.real()) < tol) && (std::abs(a.imag() - b.imag()) < tol);
}

} // namespace

TEST_CASE("UniformPlanarSquareArray 2×2, 5 cm spacing", "[UniformPlanarSquareArray]") {
    UniformPlanarSquareArray<4> sensor(0.05f);

    WHEN("azimuth 0, 1000 Hz — phase depends only on column i") {
        auto sv = sensor.getSteeringVector(0.0f, 1000.0f);

        const float lambda = 343.0f / 1000.0f;
        const float phase1 = 2.0f * static_cast<float>(M_PI) * (0.05f / lambda);
        const auto step = std::exp(std::complex<float>(0.0f, phase1));

        REQUIRE(sv[0] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(sv[2] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(inTolerance(sv[1], step, 1e-5f));
        REQUIRE(inTolerance(sv[3], step, 1e-5f));
    }

    WHEN("azimuth pi/2, 1000 Hz — phase depends only on row j") {
        auto sv = sensor.getSteeringVector(static_cast<float>(M_PI) / 2.0f, 1000.0f);

        const float lambda = 343.0f / 1000.0f;
        const float phase1 = 2.0f * static_cast<float>(M_PI) * (0.05f / lambda);
        const auto step = std::exp(std::complex<float>(0.0f, phase1));

        REQUIRE(inTolerance(sv[0], std::complex<float>{1.0f, 0.0f}, 1e-5f));
        REQUIRE(inTolerance(sv[1], std::complex<float>{1.0f, 0.0f}, 1e-5f));
        REQUIRE(inTolerance(sv[2], step, 1e-5f));
        REQUIRE(inTolerance(sv[3], step, 1e-5f));
    }
}
