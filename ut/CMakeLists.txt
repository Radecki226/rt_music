cmake_minimum_required(VERSION 3.16)
project(ut CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TEST_DIR ${CMAKE_SOURCE_DIR})
set(PROJ_DIR ${CMAKE_SOURCE_DIR}/..)

include_directories(${PROJ_DIR}/src/circularBuffer)

add_executable(ut 
    ${TEST_DIR}/TestCircularBuffer.cpp
    ${PROJ_DIR}/src/circularBuffer/CircularBuffer.hpp 
)

target_compile_options(ut PRIVATE
    -Wall
    -Wextra
    -Werror
)

target_compile_options(ut PUBLIC 
    -fsanitize=address 
    -fno-omit-frame-pointer 
)

target_link_options(ut PRIVATE
    -fsanitize=address
)
