#pragma once

#include "Buffer.hpp"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

template <typename InputIt>
Buffer::Buffer(InputIt first, InputIt last) : Buffer(std::distance(first, last))
{
    (void)std::copy(first, last, begin());
    _filled_size = size();
}

template <typename InputIt>
void Buffer::replace(InputIt first, InputIt last)
{
    size_t new_size = std::distance(first, last);
    if (new_size > size())
    {
        throw std::out_of_range("Buffer::replace: iterator too large");
    }
    (void)std::copy(first, last, begin());
    _filled_size = new_size;
}
