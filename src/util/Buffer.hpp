#pragma once

#include <vector>
#include <cstddef>

/// A vector wrapper to keep track of filled/unfilled portions of a buffer.
class Buffer
{
    public:
        using Container = std::vector<char>;
        using iterator = Container::iterator;
        using const_iterator = Container::const_iterator;

        /// Constructs the buffer with `size`.
        Buffer(size_t size);

        // TODO deprecate
        explicit Buffer(Container&& container) noexcept;
        explicit Buffer(const Container& container);

        // Range constructor
        template <typename InputIt>
        Buffer(InputIt first, InputIt last);

        /// Replace buffer contents by copying the given range to the beginning.
        ///
        /// This function behaves effectively like reconstructing the buffer without reallocating.
        ///
        /// @throws out_of_range - Input range is larger than `size()`
        /// @warn behavior is undefined if `data()` points in range `first` - `last`
        template <typename InputIt>
        void replace(InputIt first, InputIt last);

        /// Clears the buffer, setting `filled_size` to 0.
        void clear();

        /// Returns an iterator to the beginning of the buffer's filled portion.
        iterator       begin() noexcept;
        const_iterator begin() const noexcept;

        /// Returns an iterator pointing past the end of the buffer's filled portion.
        iterator       end() noexcept;
        const_iterator end() const noexcept;

        /// Returns the number of filled bytes in the buffer.
        size_t filled_size() const;

        /// Returns the number of unfilled bytes in the buffer.
        size_t unfilled_size() const;

        /// Returns true if the buffer is completely filled.
        bool is_full() const;

        /// Returns the total size of the buffer.
        size_t size() const;

        /// Advances the filled portion of the buffer by `count` bytes.
        void advance(size_t count);

        /// Returns a raw pointer to the unfilled portion of the buffer.
        char*       filled();
        const char* filled() const;

        /// Returns a pointer to the unfilled portion of the buffer.
        char*       unfilled();
        const char* unfilled() const;

        /// Returns the internal container of the buffer.
        Container container() &&;

    private:
        Container _container;
        size_t    _filled_size = 0;
};

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

#ifdef TEST

class BufferTest : public Buffer
{
    public:
        static void range_constructor_test();
        static void iterator_basic_test();
        static void iterator_empty_test();
        static void iterator_full_test();
};

#endif
