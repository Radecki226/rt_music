#include <catch2/catch_test_macros.hpp>
#include "CircularBuffer.hpp"

#include <cstdint>
#include <vector>
#include <new>

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

        CircularBuffer<2> newBuffer(1);
        REQUIRE(newBuffer.getCurrentSizeChunks() == 0);
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

    WHEN("Applying reconfig") {
        newBuffer.push(std::array<int32_t, 2>{0, 0});
        REQUIRE(newBuffer.getCurrentSizeChunks() == 1);
        newBuffer.reconfig(1);
        REQUIRE(newBuffer.getCurrentSizeChunks() == 0);
    }
}

TEST_CASE("CircularBuffer Try Get Data", "[CircularBuffer]") {
    CircularBuffer<2> newBuffer(3);
    std::vector<int32_t> outVector;

    WHEN("Trying to get more chunks than output size") {
        std::array<int32_t, 2> dummyArray = {1, 2};
        newBuffer.push(dummyArray);
        newBuffer.push(dummyArray);
        outVector.resize(2);

        REQUIRE_THROWS_AS(
            newBuffer.get(2, outVector),
            std::invalid_argument
        );
    }

    WHEN("Trying to get 0 chunks") {
        std::array<int32_t, 2> dummyArray = {1, 2};
        newBuffer.push(dummyArray);
        outVector.resize(2);

        REQUIRE_THROWS_AS(
            newBuffer.get(0, outVector),
            std::invalid_argument
        );
    }

    WHEN("Trying to get more chunks that are stored") {
        std::array<int32_t, 2> dummyArray = {1, 2};
        newBuffer.push(dummyArray);
        outVector.resize(4);

        REQUIRE_THROWS_AS(
            newBuffer.get(2, outVector),
            std::invalid_argument
        );
    }

    WHEN("Trying to get elements from continous range") {
        outVector.resize(100);
        std::array<int32_t, 2> dummyArray = {21, 37};
        newBuffer.push(dummyArray);
        
        REQUIRE_NOTHROW(
            newBuffer.get(1, outVector)
        );

        REQUIRE(outVector[0] == 21);
        REQUIRE(outVector[1] == 37);
    }

    WHEN("Trying to get elements from non-continous range") {
        outVector.resize(100);
        newBuffer.push(std::array<int32_t, 2>{1, 2});
        newBuffer.push(std::array<int32_t, 2>{3, 4});
        newBuffer.push(std::array<int32_t, 2>{5, 6});

        WHEN("Buffer full") { THEN("Overwrite beginning") {
            newBuffer.push(std::array<int32_t, 2>{7, 8});
            
            REQUIRE_NOTHROW(
                newBuffer.get(2, outVector)
            );

            REQUIRE(outVector[0] == 5);
            REQUIRE(outVector[1] == 6);
            REQUIRE(outVector[2] == 7);
            REQUIRE(outVector[3] == 8);
        }}
    }
}
