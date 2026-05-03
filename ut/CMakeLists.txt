cmake_minimum_required(VERSION 3.15)

project(unitTests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(Catch2 REQUIRED) 
include(CTest)
include(Catch)

add_library(StrictBuild INTERFACE)
set(STRICT_WARNINGS 
    -Wall 
    -Wextra
    -Wpedantic
    -Wno-sign-compare
    -Wno-psabi
)
set(ASAN_FLAGS 
    -fsanitize=address 
    -fno-omit-frame-pointer # Better stack traces
)

target_compile_options(StrictBuild INTERFACE ${STRICT_WARNINGS} ${ASAN_FLAGS})
target_link_options(StrictBuild INTERFACE ${ASAN_FLAGS})


add_executable(unitTests
    TestCircularBuffer.cpp
    TestDspMusic.cpp
    TestUniformPlanarSquareArray.cpp
    TestSingleFrequencySystem.cpp
    TestSingleFrequencySystemIntegration.cpp
)

target_link_libraries(unitTests PRIVATE
    Catch2::Catch2WithMain
    StrictBuild
)

target_include_directories(unitTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../data/steering_vectors
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/buffer
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dsp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/steeringVector
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/system
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/testEnv
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/eigen
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/trompeloeil/include
)

target_compile_definitions(unitTests PRIVATE
    TROMPELOEIL_BUILD_TESTS=yes
)

catch_discover_tests(unitTests)
