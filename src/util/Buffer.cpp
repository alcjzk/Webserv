#include <limits>
#include <utility>
#include <cstddef>
#include "Buffer.hpp"

using Container = Buffer::Container;

using iterator = Buffer::iterator;
using const_iterator = Buffer::const_iterator;

Buffer::Buffer(size_t size) : _container(size) {}

Buffer::Buffer(Container&& container) noexcept : _container(std::move(container))
{
    _filled_size = _container.size();
}

Buffer::Buffer(const Container& container) : _container(container)
{
    _filled_size = container.size();
}

void Buffer::clear()
{
    _filled_size = 0;
}

iterator Buffer::begin() noexcept
{
    return _container.begin();
}

const_iterator Buffer::begin() const noexcept
{
    return _container.begin();
}

iterator Buffer::end() noexcept
{
    return _container.begin() + _filled_size;
}

const_iterator Buffer::end() const noexcept
{
    return _container.begin() + _filled_size;
}

size_t Buffer::filled_size() const
{
    return _filled_size;
}

size_t Buffer::unfilled_size() const
{
    return _container.size() - _filled_size;
}

bool Buffer::is_full() const
{
    return _filled_size == _container.size();
}

size_t Buffer::size() const
{
    return _container.size();
}

void Buffer::advance(size_t count)
{
    _filled_size += count;
}

char* Buffer::filled()
{
    return _container.data();
}

char* Buffer::unfilled()
{
    return _container.data() + _filled_size;
}

Container Buffer::container() &&
{
    return std::move(_container);
}

void Buffer::reserve(size_t count)
{
    if (count > unfilled_size())
    {
        size_t additional_size = count - unfilled_size();
        _container.resize(_container.size() + additional_size);
    }
}

bool Buffer::grow(size_t max_capacity)
{
    if (_container.size() >= max_capacity)
        return false;
    if (std::numeric_limits<size_t>::max() - _container.size() < _container.size())
        return false;
    _container.resize(std::min(_container.size() * 2, max_capacity));
    return true;
}

#ifdef TEST

#include <iterator>
#include <string>
#include "testutils.hpp"

void BufferTest::range_constructor_test()
{
    BEGIN

    std::string str("abc");

    Buffer buffer(str.begin(), str.end());
    EXPECT(buffer.filled_size() == 3);
    EXPECT(buffer.size() == 3);

    END
}

void BufferTest::iterator_basic_test()
{
    BEGIN

    Buffer buffer(4);
    buffer.advance(2);
    EXPECT(std::distance(buffer.begin(), buffer.end()) == 2);

    END
}

void BufferTest::iterator_empty_test()
{
    BEGIN

    Buffer buffer_unfilled(4);
    Buffer buffer_zerosized(0);

    EXPECT(std::distance(buffer_unfilled.begin(), buffer_unfilled.end()) == 0);
    EXPECT(std::distance(buffer_zerosized.begin(), buffer_zerosized.end()) == 0);

    END
}

void BufferTest::iterator_full_test()
{
    BEGIN

    Buffer buffer(4);
    buffer.advance(4);

    EXPECT(std::distance(buffer.begin(), buffer.end()) == 4);

    END
}

#endif
