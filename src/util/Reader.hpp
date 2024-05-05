#pragma once

#include <limits>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <limits>
#include <optional>
#include <cstddef>
#include <vector>
#include "Buffer.hpp"

class Reader
{
    public:
        using iterator = Buffer::iterator;
        using const_iterator = Buffer::const_iterator;

        class LineLimitError : public std::runtime_error
        {
            public:
                virtual const char* what() const noexcept override;
        };

        /// Constructs the reader from an existing buffer.
        Reader(Buffer&& buffer);

        /// Constructs the reader with a new buffer.
        Reader(size_t buffer_size);

        /// Extracts a line from the buffer, advancing the reader accordinly.
        ///
        /// @param limit - Maximum length of the line, not including the linefeed sequence.
        ///
        /// @throws std::runtime_exception if the line is too long.
        ///
        /// Any CR not followed by a LF is converted to a SP. The terminating CRLF/LF is removed
        /// from the returned line.
        std::optional<std::string> line(size_t limit = std::numeric_limits<size_t>::max());

        /// Advances the reader from the current position, skipping any empty
        /// lines (CRLF/LF).
        ///
        /// If there are no empty lines at the current
        /// position or the reader has been exhausted, this function has no
        /// effect.
        void trim_empty_lines();

        /// Returns exactly `count` bytes from the internal buffer, advancing the reader.
        ///
        /// If the buffer does not contain at least `count` unread bytes, the returned vector will
        /// be empty.
        std::vector<char> read_exact(size_t count);

        /// Reads exactly `count` bytes from the internal buffer into destination range is not large
        /// enough.
        ///
        /// @returns
        /// true - read was successful
        /// false - reader did not contain enough unread bytes.
        ///
        /// @note behavior is undefined if destination range is not large enough.
        template <typename OutputIt>
        bool read_exact_into(size_t count, OutputIt destination);

        /// Returns a reference to the internal buffer.
        Buffer&       buffer() &;
        Buffer&&      buffer() &&;
        const Buffer& buffer() const&;

        /// Replaces the internal buffer.
        void buffer(Buffer&& buffer);

        /// Returns the number of unread bytes in the Reader.
        size_t unread_size() const;

        /// Returns an iterator to the beginning of the unread content.
        ///
        /// @note The returned iterators will never advance the reader.
        iterator       begin();
        const_iterator begin() const;

        /// Returns an iterator past the end of the unread content.
        ///
        /// @note The returned iterators will never advance the reader.
        iterator       end();
        const_iterator end() const;

        /// Rewinds the reader to the beginning.
        void rewind();

        /// Returns true if the reader has no unread content.
        bool is_empty() const;

        /// Reserves space or `count` bytes bytes in the underlying buffer's unread portion.
        void reserve(size_t count);

        /// Advances the reader by `count` bytes.
        ///
        /// Behavior is undefined if reader does not contain at least `count` unread bytes.
        void advance(size_t count);

        /// Grows the internal buffer by doubling the capacity or up to `max_capacity`.
        ///
        /// @returns
        /// true - buffer was resized.
        /// false - buffer size would exceed max_capacity or numeric limits.
        ///
        /// @throws see std::vector::resize.
        bool grow(size_t max_capacity);

        /// Returns the position of the first occurrance of pattern `first`-`last`.
        template <typename ForwardIt>
        std::optional<size_t> position(ForwardIt first, ForwardIt last) const;

        /// Advances the reader to the beginning of the first occurrance of pattern `first`-`last`.
        ///
        /// @returns
        /// true - pattern found, reader was advanced.
        /// false - pattern was not found, reader not advanced.
        template <typename ForwardIt>
        bool seek(ForwardIt first, ForwardIt last);

    private:
        Buffer           _buffer;
        Buffer::iterator _head;
};

template <typename OutputIt>
bool Reader::read_exact_into(size_t count, OutputIt destination)
{
    if (unread_size() < count)
        return false;
    (void)std::copy_n(begin(), count, destination);
    std::advance(_head, count);
    return true;
}

template <typename ForwardIt>
std::optional<size_t> Reader::position(ForwardIt first, ForwardIt last) const
{
    auto it = std::search(begin(), end(), first, last);
    if (it == end())
        return std::nullopt;
    return std::distance(static_cast<const_iterator>(_head), it);
}

template <typename ForwardIt>
bool Reader::seek(ForwardIt first, ForwardIt last)
{
    if (auto pos = position(first, last))
    {
        std::advance(_head, *pos);
        return true;
    }
    return false;
}

#ifdef TEST

class ReaderTest : public Reader
{
    public:
        static void line_empty_test();
        static void line_basic_test();
        static void line_noline_test();
        static void line_strip_bare_cr_test();
        static void trim_empty_lines_test();
        static void line_limit_test();
        static void read_exact_basic_test();
        static void read_exact_empty_test();

    private:
        static Buffer buffer(const std::string& content);
};

#endif
