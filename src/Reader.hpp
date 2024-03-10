#pragma once

#include <limits>
#include <vector>
#include <string>
#include <limits>
#include <optional>

class Reader
{
    public:
        /// Constructs the reader by copying a buffer.
        Reader(const std::vector<char>& buffer);

        /// Constructs the reader by moving a buffer.
        Reader(std::vector<char>&& buffer);

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
        void                       trim_empty_lines();

        /// Returns a raw pointer to the internal buffer.
        char*                      data() noexcept;

    private:
        std::vector<char>           _buffer;
        std::vector<char>::iterator _head;
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
        static std::vector<char> buffer(const char* content);
};

#endif
