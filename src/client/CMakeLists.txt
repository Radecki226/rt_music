cmake_minimum_required(VERSION 3.10)
project(music-app LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)

add_executable(music-app musicDspClient.cpp)

target_link_libraries(music-app 
    PRIVATE 
    MusicDsp 
    Eigen3::Eigen
)

install(TARGETS music-app DESTINATION bin)
