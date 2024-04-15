#pragma once

#include "Buffer.hpp"
#include <algorithm>

template <typename InputIt>
Buffer::Buffer(InputIt first, InputIt last) : Buffer(std::distance(first, last))
{
    std::copy(first, last, begin());
    _filled_size = size();
}

