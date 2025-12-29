cmake_minimum_required(VERSION 3.15)

project(unitTests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch2 REQUIRED) 
include(CTest)
include(Catch)

add_library(StrictBuild INTERFACE)
set(STRICT_WARNINGS 
    -Wall 
    -Wextra
    -Wpedantic
    -Werror
    -Wno-sign-compare
    -Wno-psabi
)
set(ASAN_FLAGS 
    -fsanitize=address 
    -fno-omit-frame-pointer # Better stack traces
)

target_compile_options(StrictBuild INTERFACE ${STRICT_WARNINGS} ${ASAN_FLAGS})
target_link_options(StrictBuild INTERFACE ${ASAN_FLAGS})


# add_library(CircularBuffer STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../src/buffer/CircularBuffer.hpp)
# target_include_directories(CircularBuffer INTERFACE 
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src
# )

add_library(CircularBuffer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../src/buffer/CircularBuffer.hpp)
target_link_libraries(CircularBuffer INTERFACE StrictBuild)
target_include_directories(CircularBuffer INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/buffer
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/eigen
)

add_library(DspMusic INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../src/dsp/DspMusic.hpp)
target_link_libraries(DspMusic INTERFACE StrictBuild)
target_include_directories(DspMusic INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dsp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/eigen
)

add_executable(unitTests
    TestCircularBuffer.cpp
    TestDspMusic.cpp
)

target_link_libraries(unitTests PRIVATE
    CircularBuffer
    DspMusic
    Catch2::Catch2WithMain
    StrictBuild
)

catch_discover_tests(unitTests)
