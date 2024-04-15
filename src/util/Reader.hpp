#pragma once

#include <limits>
#include <stdexcept>
#include <string>
#include <limits>
#include <optional>
#include "Buffer.hpp"

class Reader
{
    public:
        class LineLimitError : public std::runtime_error
        {
            public:
                virtual const char* what() const noexcept override;
        };

        /// Constructs the reader from an existing buffer.
        Reader(Buffer&& buffer);

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

        /// Returns a reference to the internal buffer.
        Buffer&       buffer();
        const Buffer& buffer() const;

    private:
        Buffer           _buffer;
        Buffer::iterator _head;
};

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

    private:
        static Buffer buffer(const std::string& content);
};

#endif
