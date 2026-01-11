SUMMARY = "A small image for Music DSP that mirrors core-image-minimal"
LICENSE = "MIT"

# This pulls in all the logic from core-image-minimal
require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL += " \
    music-dsp \
    music-client \
    "

# Optional: Add extra space on the SD card/EMMC for data logs (e.g., 50MB)
# IMAGE_ROOTFS_EXTRA_SPACE = "51200"
