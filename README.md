# rt_music
RT Music implementation in c++.

## Catch2
```
cd ext/Catch2
cmake -B Build -S . -DBUILD_TESTING=OFF
sudo cmake --build Build/ --target install
```

## Build ut
```
mkdir ut/build && cd ut/build
cmake -GNinja ..
ninja && ctest -R ".*"
```