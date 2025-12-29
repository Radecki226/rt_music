#include <catch2/catch_test_macros.hpp>
#include "UniformLinearArray.hpp"
#include <complex>

bool inTolerance(const std::complex<float>& a, const std::complex<float>& b, float tol) {
    return (std::abs(a.real() - b.real()) < tol) && (std::abs(a.imag() - b.imag()) < tol);
}

TEST_CASE( "UniformLinearArray 4 microphones, 5cm spacing", "[UniformLinearArray]" ) {
    UniformLinearArray<4> sensor(0.05f);

    WHEN("0 degrees, 1000Hz") {
        auto sv = sensor.getSteeringVector(0.0f, 1000.0f);

        REQUIRE(sv[0] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(sv[1] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(sv[2] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(sv[3] == std::complex<float>{1.0f, 0.0f});
    }

    WHEN("30 degrees, 1000Hz") {
        auto sv = sensor.getSteeringVector(M_PI / 6.0f, 1000.0f);

        float lambda = 343.0f / 1000.0f;
        float deltaD = 0.05f * std::sin(M_PI / 6.0f);
        float deltaPhi = 2.0f * M_PI * (deltaD / lambda);
        std::complex<float> expected_step = std::exp(std::complex<float>(0, deltaPhi));

        REQUIRE(sv[0] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(inTolerance(sv[1], expected_step, 1e-5f));
        REQUIRE(inTolerance(sv[2], expected_step * expected_step, 1e-5f));
        REQUIRE(inTolerance(sv[3], expected_step * expected_step * expected_step, 1e-5f));
    }
}
