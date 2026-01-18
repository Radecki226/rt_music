SUMMARY = "A small image for Music DSP that mirrors core-image-minimal"
LICENSE = "MIT"

# This pulls in all the logic from core-image-minimal
require recipes-core/images/core-image-minimal.bb

IMAGE_FEATURES += "ssh-server-dropbear debug-tweaks"

IMAGE_INSTALL += " \
    music-dsp \
    music-client \
    iw \
    wpa-supplicant \
    linux-firmware-wl18xx \
    kernel-module-wlcore-sdio \
    kernel-module-wl18xx \
    "

# Optional: Add extra space on the SD card/EMMC for data logs (e.g., 50MB)
# IMAGE_ROOTFS_EXTRA_SPACE = "51200"
