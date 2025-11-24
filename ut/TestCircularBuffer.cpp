#include <cstdio>
#include "CircularBuffer.hpp"

int main() {
    printf("Hello world!\n");

    std::vector<int32_t> out;
    out.resize(10);

    CircularBuffer<7> buffer(3);
    buffer.push(std::array<int32_t, 7>{0});
    buffer.get(1, out);
}
