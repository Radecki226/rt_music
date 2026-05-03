#include <catch2/catch_test_macros.hpp>
#include "DspMusic.hpp"

bool inTolerance(float a, float b, float tol) {
    return (std::abs(a-b) < tol);
}

TEST_CASE( "DspMusic Construct", "[DspMusic]" ) {
    WHEN("Signal subspace dimension 0") {
        REQUIRE_THROWS_AS(
            DspMusic<2>(0, 8),
            std::invalid_argument
        );
    }

    WHEN("Signal subspace dimension equal to number of microphones") {
        REQUIRE_THROWS_AS(
            DspMusic<2>(2, 8),
            std::invalid_argument
        );
    }

    WHEN("Proper signal subspace dimension") {
        REQUIRE_NOTHROW(
            DspMusic<2>(1, 8)
        );
    }
}

TEST_CASE("DspMusic Reconfig", "[DspMusic]") {
    DspMusic<2> dspMusic(1, 8);
    WHEN("Signal subspace dimension 0") {
        REQUIRE_THROWS_AS(
            dspMusic.reconfig(0, 8),
            std::invalid_argument
        );
    }

    WHEN("Signal subspace dimension equal to number of microphones") {
        REQUIRE_THROWS_AS(
            dspMusic.reconfig(2, 8),
            std::invalid_argument
        );
    }

    WHEN("nAngles is 0") {
        REQUIRE_THROWS_AS(
            dspMusic.reconfig(1, 0),
            std::invalid_argument
        );
    }

    WHEN("Proper signal subspace dimension") {
        REQUIRE_NOTHROW(
            dspMusic.reconfig(1, 8)
        );
    }
}

TEST_CASE("DspMusic Compute Noise Space", "[DspMusic]") {
    DspMusic<3> dspMusic(2, 4);

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

TEST_CASE("DspMusic Calculate Pseudospectrum Batch", "[DspMusic]") {
    DspMusic<3> dspMusic(1, 2);

    using C = std::complex<float>;

    // Create steering vectors for 2 angles (columns)
    Eigen::Matrix<std::complex<float>, 3, 2> steeringVectors;
    steeringVectors.col(0) << C(1.0, 0.0), C(0.0, 1.0), C(1.0, 0.0);
    steeringVectors.col(1) << C(0.5, 0.5), C(1.0, 0.0), C(0.0, 1.0);

    Eigen::Matrix<std::complex<float>, 3, Eigen::Dynamic> noiseSpace(3, 2);
    // Column 0: eigenvector for noise subspace
    noiseSpace.col(0) << C(1.0, 0.0), C(0.0, 0.0), C(0.0, 0.0);
    // Column 1: eigenvector for noise subspace  
    noiseSpace.col(1) << C(1.0, 0.0), C(1.0, 0.0), C(1.0, 0.0);

    WHEN("Proper noise space matrix size") {
        Eigen::Matrix<float, Eigen::Dynamic, 1> pseudospectra(2);
        dspMusic.calculatePseudospectrumBatch(steeringVectors, noiseSpace, pseudospectra);
        REQUIRE(pseudospectra.size() == 2);
        printf("Pseudospectrum values: %f, %f\n", pseudospectra(0), pseudospectra(1));
        REQUIRE(inTolerance(pseudospectra(0), (1.0f / 6.0f), (float)1e-6));
        REQUIRE(inTolerance(pseudospectra(1), (1.0f / 5.0f), (float)1e-6));
    }

    WHEN("Improper noise space matrix size") {
        Eigen::Matrix<std::complex<float>, 3, Eigen::Dynamic> badNoiseSpace(3, 1);
        Eigen::Matrix<float, Eigen::Dynamic, 1> out(2);
        REQUIRE_THROWS_AS(
            dspMusic.calculatePseudospectrumBatch(steeringVectors, badNoiseSpace, out),
            std::invalid_argument
        );
    }

    WHEN("Output pseudospectrum wrong length") {
        Eigen::Matrix<float, Eigen::Dynamic, 1> out(1);
        REQUIRE_THROWS_AS(
            dspMusic.calculatePseudospectrumBatch(steeringVectors, noiseSpace, out),
            std::invalid_argument
        );
    }
}
