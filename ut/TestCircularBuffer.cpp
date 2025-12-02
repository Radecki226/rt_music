#include <catch2/catch_test_macros.hpp>
#include "CircularBuffer.hpp"

#include <cstdint>
#include <new>
#include <array>
#include <complex>

TEST_CASE( "CircularBuffer Construct", "[CircularBuffer]" ) {
    WHEN("Size 0") {
        REQUIRE_THROWS_AS(
            CircularBuffer<2>(0),
            std::invalid_argument
        );
    }

    WHEN("Proper Size") {
        REQUIRE_NOTHROW(
            CircularBuffer<2>(1)
        );
    }
}

TEST_CASE("CircularBuffer Reconfig", "[CircularBuffer]") {
    CircularBuffer<2> newBuffer(1);
    WHEN("Size 0") {
        REQUIRE_THROWS_AS(
            newBuffer.reconfig(0),
            std::invalid_argument
        );
    }

    WHEN("Proper size") {
        REQUIRE_NOTHROW(
            newBuffer.reconfig(1)
        );
    }
}

TEST_CASE("CircularBuffer Calc from one real sample", "[CircularBuffer]") {
    CircularBuffer<2> newBuffer(3);
    std::array<std::complex<float>, 2> data{-1.0f, 2.0f};
    newBuffer.push(data);

    Eigen::Matrix<std::complex<float>, 2, 2> cov;
    newBuffer.calcCov(cov);

    const float one_third = 1.0f/3;
    REQUIRE(cov(0, 0) == std::complex<float>{one_third, 0.0});
    REQUIRE(cov(0, 1) == std::complex<float>{-2*one_third, 0.0});
    REQUIRE(cov(1, 0) == std::complex<float>{-2*one_third, 0.0});
    REQUIRE(cov(1, 1) == std::complex<float>{4*one_third, 0.0});
}

TEST_CASE("CircularBuffer Calc from two complex samples", "[CircularBuffer]") {
    CircularBuffer<2> newBuffer(2);
    std::array<std::complex<float>, 2> outdated{-1.0, 2.0};
    std::array<std::complex<float>, 2> first = {
        std::complex<float>{1.0f, 2.0f},
        std::complex<float>{1.0f, 2.0f},
    };     

    std::array<std::complex<float>, 2> second = {
        std::complex<float>{3.0f, 2.0f},
        std::complex<float>{3.0f, 4.0f},   
    };

    //This sample should be discarded since buffer has a memory of 2 samples
    newBuffer.push(outdated);
    newBuffer.push(first);
    newBuffer.push(second);

    Eigen::Matrix<std::complex<float>, 2, 2> cov;
    newBuffer.calcCov(cov);

    REQUIRE(cov(0, 0) == std::complex<float>{9.0f, 0});
    REQUIRE(cov(0, 1) == std::complex<float>{11.0f, -3.0f});
    REQUIRE(cov(1, 0) == std::complex<float>{11.0f, 3.0f});
    REQUIRE(cov(1, 1) == std::complex<float>{15.0f, 0});
}
