SUMMARY = "Music DSP Client Application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# FIX 1: Combine dependencies so both are included
DEPENDS = "music-dsp libeigen"

FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../:"

SRC_URI = "file://src/client/"
S = "${WORKDIR}/src/client"

do_compile() {
    # FIX 2: Add -I${STAGING_INCDIR} 
    # This allows the compiler to find <music-dsp/musicDsp.hpp>
    ${CXX} ${CXXFLAGS} musicDspClient.cpp ${LDFLAGS} \
    -I${STAGING_INCDIR} \
    -I${STAGING_INCDIR}/eigen3 \
    -lMusicDsp \
    -o music-app
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 music-app ${D}${bindir}
}