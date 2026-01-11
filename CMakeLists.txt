cmake_minimum_required(VERSION 3.15)
project(MusicDsp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Eigen3 REQUIRED)

add_library(MusicDsp SHARED 
    src/lib/musicDsp.cpp
)

target_include_directories(MusicDsp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/lib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/buffer>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src//dsp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/steeringVector>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/system>
    $<INSTALL_INTERFACE:include/music-dsp>
)

target_link_libraries(MusicDsp PUBLIC Eigen3::Eigen)

set_target_properties(MusicDsp PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

install(TARGETS MusicDsp
    EXPORT MusicDspTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/music-dsp
)

# Install the header files so you can build apps against the library later
install(FILES 
    src/lib/musicDsp.hpp 
    DESTINATION include/music-dsp
)
