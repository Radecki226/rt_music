cmake_minimum_required(VERSION 3.15)

project(unitTests CXX C)

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
    -Werror
)
set(ASAN_FLAGS 
    -fsanitize=address 
    -fno-omit-frame-pointer # Better stack traces
)

target_compile_options(StrictBuild INTERFACE ${STRICT_WARNINGS} ${ASAN_FLAGS})
target_link_options(StrictBuild INTERFACE ${ASAN_FLAGS})


add_executable(unitTests
    ${CMAKE_CURRENT_SOURCE_DIR}/TestCircularBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestDspMusic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestUniformPlanarSquareArray.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestSingleFrequencySystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestSingleFrequencySystemIntegration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestMultiFrequencySystemIntegration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CustomMalloc.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/kissfft
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/trompeloeil/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(unitTests PRIVATE
    TROMPELOEIL_BUILD_TESTS=yes
)

set(KISSFFT_C ${CMAKE_CURRENT_SOURCE_DIR}/../ext/kissfft/kiss_fft.c)
set_source_files_properties(${KISSFFT_C} PROPERTIES LANGUAGE C)
target_sources(unitTests PRIVATE ${KISSFFT_C})

catch_discover_tests(unitTests)
