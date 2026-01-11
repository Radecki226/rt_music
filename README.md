# rt_music
RT Music implementation in c++.

## Description
Goal of this project is to implement real-time version of MUSIC algorithm that runs on Beagleplay platform.
System shall be capable of detecting direction of one source for human speech signal.

## Getting Started

### Catch2
```
cd ext/Catch2
cmake -B Build -S . -DBUILD_TESTING=OFF
sudo cmake --build Build/ --target install
```

### Build ut
```
mkdir ut/build && cd ut/build
cmake -GNinja ..
ninja && ctest -R ".*"
```

### Yocto
```
source poky/oe-init-build-env
bitbake-layers add-layer ../meta-arm/meta-arm-toolchain
bitbake-layers add-layer ../meta-arm/meta-arm
bitbake-layers add-layer ../meta-ti/meta-ti-bsp
bitbake-layers add-layer ../meta-music
```

Then choose correct machine in your local.conf
```
MACHINE ?= "beagleplay-ti"
```