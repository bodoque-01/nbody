#pragma once

#include <cstddef>
#include <vector>
#include "raylib.h"

class CircularBuffer {
public:
    explicit CircularBuffer(std::size_t capacity);

    void push(Vector2 value);
    Vector2 at(std::size_t index) const;
    Vector2 latest() const;

    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
    bool full() const noexcept;

    void clear() noexcept;

private:
    std::vector<Vector2> buffer_;
    std::size_t head_;
    std::size_t size_;
};
