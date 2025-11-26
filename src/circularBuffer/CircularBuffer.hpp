#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <mutex>
#include <array>

template <size_t M>
class CircularBuffer {    
private:
    static_assert(
        M > 0, 
        "Error: Number of microphones can't be 0!"
    );

    std::vector<int32_t> buffer_;
    size_t capacityChunks_ = 0;    
    size_t headChunk_ = 0;
    bool isFull_ = false;

    mutable std::mutex accessMutex_;

public:
    explicit CircularBuffer(size_t initialCapacityChunks);

    void reconfig(size_t newCapacityChunks);

    void push(const std::array<int32_t, M> &chunk);

    void get(size_t numChunksToGet, std::vector<int32_t>& destinationBuffer) const;

    size_t getCurrentSizeChunks() const;
};

template <size_t M>
CircularBuffer<M>::CircularBuffer(size_t initialCapacityChunks) {
    reconfig(initialCapacityChunks);
}

template <size_t M>
void CircularBuffer<M>::reconfig(size_t newCapacityChunks) {
    if (newCapacityChunks == 0) {
        throw std::invalid_argument("Capacity and M must be greater than zero.");
    }

    std::lock_guard<std::mutex> lock(accessMutex_);

    capacityChunks_ = newCapacityChunks;
    size_t totalElements = M * capacityChunks_;

    buffer_.resize(totalElements);
    std::memset(buffer_.data(), 0, sizeof(int32_t) * totalElements);

    headChunk_ = 0;
    isFull_ = false;
}

template <size_t M>
void CircularBuffer<M>::push(const std::array<int32_t, M> &array) {
    std::lock_guard<std::mutex> lock(accessMutex_); 

    size_t startIndex = headChunk_ * M;

    std::copy(array.begin(), array.end(), buffer_.begin() + startIndex);

    headChunk_ = (headChunk_ + 1) % capacityChunks_;

    if (headChunk_ == 0 && !isFull_) {
        isFull_ = true;
    }
}

template <size_t M>
void CircularBuffer<M>::get(size_t numChunksToGet, std::vector<int32_t>& destinationBuffer) const {
    if (numChunksToGet*M > destinationBuffer.size() || numChunksToGet == 0) {
        throw std::invalid_argument("Mismatch between buffer size and requested length!");
    }

    std::lock_guard<std::mutex> lock(accessMutex_); 

    size_t currentSizeChunks = getCurrentSizeChunks();

    if (numChunksToGet > currentSizeChunks) {
        throw std::invalid_argument("Requested chunk bigger than circular buffer size");
    }

    size_t startIdx;
    if (headChunk_ > numChunksToGet) {
        startIdx = (headChunk_- numChunksToGet) * M;
    } else {
        startIdx = (capacityChunks_ - numChunksToGet + headChunk_) * M;
    }

    size_t capacityWords = capacityChunks_ * M;

    for (size_t i = 0; i < numChunksToGet*M; i++) {
        destinationBuffer[i] = buffer_[(startIdx+i) % capacityWords];
    }
}

template <size_t M>
size_t CircularBuffer<M>::getCurrentSizeChunks() const { 
    return isFull_ ? capacityChunks_ : headChunk_;
}
