#include "circular_buffer.h"

#include <stdexcept>

CircularBuffer::CircularBuffer(std::size_t capacity)
    : buffer_(capacity, Vector2{0.0f, 0.0f}), head_(0), size_(0) {
    if (capacity == 0) {
        throw std::invalid_argument("CircularBuffer capacity must be greater than zero");
    }
}

void CircularBuffer::push(Vector2 value) {
    if (size_ < buffer_.size()) {
        std::size_t writeIndex = (head_ + size_) % buffer_.size();
        buffer_[writeIndex] = value;
        ++size_;
        return;
    }

    buffer_[head_] = value;
    head_ = (head_ + 1) % buffer_.size();
}

Vector2 CircularBuffer::at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("CircularBuffer index out of range");
    }

    std::size_t realIndex = (head_ + index) % buffer_.size();
    return buffer_[realIndex];
}

Vector2 CircularBuffer::latest() const {
    if (empty()) {
        throw std::out_of_range("CircularBuffer is empty");
    }

    std::size_t latestIndex = (head_ + size_ - 1) % buffer_.size();
    return buffer_[latestIndex];
}

std::size_t CircularBuffer::size() const noexcept {
    return size_;
}

std::size_t CircularBuffer::capacity() const noexcept {
    return buffer_.size();
}

bool CircularBuffer::empty() const noexcept {
    return size_ == 0;
}

bool CircularBuffer::full() const noexcept {
    return size_ == buffer_.size();
}

void CircularBuffer::clear() noexcept {
    head_ = 0;
    size_ = 0;
}
