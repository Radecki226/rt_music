SUMMARY = "Shared library for Music DSP"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "libeigen"

# Use the 'FILESEXTRAPATHS' variable to tell BitBake where to look for files
# This adds your project root to the search path
FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../:"

# Now you can list the files simply by name
SRC_URI = " \
    file://CMakeLists.txt \
    file://src/ \
"

# Set S to WORKDIR because CMakeLists.txt is copied to the top of WORKDIR
S = "${WORKDIR}"

inherit cmake

FILES:${PN}-dev = " \
    ${includedir}/music-dsp/*.hpp \
    ${includedir}/music-dsp \
    ${libdir}/libMusicDsp.so \
"
