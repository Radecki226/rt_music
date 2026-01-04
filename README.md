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
