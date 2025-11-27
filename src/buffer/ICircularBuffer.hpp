#pragma once

#include <vector>
#include <cstdint>
#include <array>

template <size_t M>
class ICircularBuffer {
public:
    virtual ~ICircularBuffer() = default;

    /**
     * Push new chunk of data (M-len int32_t array) into the circular buffer.
     * @param chunk Array of M int32_t samples to push into the buffer.
     */
    virtual void push(const std::array<int32_t, M> &chunk) = 0;

    /**
     * Get the most recent numChunksToGet chunks from the circular buffer.
     * @param numChunksToGet Number of chunks to retrieve.
     * @param destinationBuffer Vector to store the retrieved chunks.
     */
    virtual void get(size_t numChunksToGet, std::vector<int32_t>& destinationBuffer) const = 0;
};
