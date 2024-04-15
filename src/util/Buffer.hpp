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

    private:
        Container _container;
        size_t    _filled_size = 0;
};

#include "Buffer.tpp"

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
