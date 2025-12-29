#include <catch2/catch_test_macros.hpp>
#include "DspMusic.hpp"
#include <iostream>

TEST_CASE( "DspMusic Construct", "[DspMusic]" ) {
    WHEN("Signal subspace dimension 0") {
        REQUIRE_THROWS_AS(
            DspMusic<2>(0),
            std::invalid_argument
        );
    }

    WHEN("Signal subspace dimension equal to number of microphones") {
        REQUIRE_THROWS_AS(
            DspMusic<2>(2),
            std::invalid_argument
        );
    }

    WHEN("Proper signal subspace dimension") {
        REQUIRE_NOTHROW(
            DspMusic<2>(1)
        );
    }
}

TEST_CASE("DspMusic Reconfig", "[DspMusic]") {
    DspMusic<2> dspMusic(1);
    WHEN("Signal subspace dimension 0") {
        REQUIRE_THROWS_AS(
            dspMusic.reconfig(0),
            std::invalid_argument
        );
    }

    WHEN("Signal subspace dimension equal to number of microphones") {
        REQUIRE_THROWS_AS(
            dspMusic.reconfig(2),
            std::invalid_argument
        );
    }

    WHEN("Proper signal subspace dimension") {
        REQUIRE_NOTHROW(
            dspMusic.reconfig(1)
        );
    }
}

TEST_CASE("DspMusic Compute Noise Space", "[DspMusic]") {
    DspMusic<3> dspMusic(2);

    using C = std::complex<float>;

    Eigen::Matrix3cf covMatrix;
    covMatrix << C(1.0, 0.0), C(0.0, 0.0),  C(0.0, 0.0),
                 C(0.0, 0.0), C(2.5, 0.0),  C(0.0, 0.5),
                 C(0.0, 0.0), C(0.0, -0.5), C(2.5, 0.0);
    Eigen::Matrix<std::complex<float>, 3, Eigen::Dynamic> output(3, 1);

    WHEN("Proper output matrix size") {
        REQUIRE_NOTHROW(
            dspMusic.computeNoiseSpace(output, covMatrix)
        );

        REQUIRE(output(0, 0) == C(1.0, 0.0));
        REQUIRE(output(1, 0) == C(0.0, 0.0));
        REQUIRE(output(2, 0) == C(0.0, 0.0));
    }

    WHEN("Improper output matrix size") {
        Eigen::Matrix<std::complex<float>, 3, Eigen::Dynamic> badOutput(3, 2);
        REQUIRE_THROWS_AS(
            dspMusic.computeNoiseSpace(badOutput, covMatrix),
            std::invalid_argument
        );
    }
}
