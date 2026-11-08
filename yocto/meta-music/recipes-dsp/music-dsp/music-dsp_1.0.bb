SUMMARY = "Shared library for Music DSP"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "libeigen"

# This looks back to your project root
FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../:"

# Pull in the entire src directory
# Yocto will preserve the internal structure (src/lib/...)
SRC_URI = "file://src/ file://data/steering_vectors/"

# steering_vectors.hpp is generated (gitignored): run
# precompute_steering_vectors/generate_steering_vectors.sh before bitbake.

# Set S to the location of your CMakeLists.txt
# Since you moved it to src/lib, S must match that path
S = "${WORKDIR}/src/lib"

inherit cmake

# Standard dev files configuration
FILES:${PN}-dev = " \
    ${includedir}/music-dsp/*.hpp \
    ${includedir}/music-dsp \
    ${libdir}/libMusicDsp.so \
"