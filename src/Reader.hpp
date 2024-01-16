#pragma once

#include <vector>
#include <string>
#include <exception>

class Reader
{
    public:
        Reader(const std::vector<char>& buffer);
        Reader(std::vector<char>&& buffer);

        /// Extracts a line and advances the reader accordinly
        ///
        /// @throw ReaderException::Type::NoLine
        /// @return The extracted line without CR/LF
        std::string line();

        /// Advances the reader from the current position, skipping any empty
        /// lines (CRLF/LF).
        ///
        /// If there are no empty lines at the current
        /// position or the reader has been exhausted, this function has no
        /// effect.
        void        trim_empty_lines();

        char*       data();

    private:
        std::vector<char>           _buffer;
        std::vector<char>::iterator _head;
};

class ReaderException : public std::exception
{
    public:
        typedef enum Type
        {
            NoLine
        } Type;

        ReaderException(Type type) noexcept;

        Type                type() const noexcept;

        virtual const char* what() const noexcept;

    private:
        Type _type;
};

#ifdef TEST

class ReaderTest : public Reader
{
    public:
        static void line_empty_test();
        static void line_basic_test();
        static void line_one_test();
        static void line_noline_test();
        static void line_strip_bare_cr_test();
        static void trim_empty_lines_test();

    private:
        static std::vector<char> buffer(const char* content);
};

#endif
