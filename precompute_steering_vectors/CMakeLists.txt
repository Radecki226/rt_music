cmake_minimum_required(VERSION 3.15)

project(PrecomputeSteeringVectors CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

add_executable(precompute_steering_vectors main.cpp)

# Include directories for Eigen and our headers
target_include_directories(precompute_steering_vectors PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/eigen
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/steeringVector
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)