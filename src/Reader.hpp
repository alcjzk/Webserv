#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <numeric>
#include <exception>

class Reader
{
    public:
        Reader(const std::vector<uint8_t>& buffer, size_t position = 0);

        /// @brief Advance the reader until a byte not present in charset.
        /// @param charset
        Reader& trim(const std::string& charset) throw();

        /// @brief Extracts a portion from the beginning of the reader consisting
        /// only of non-delimiting characters, advancing the reader accordingly.
        /// @param delimeters The set of characters considered as delimiters.
        /// @return String constructed from the extracted portion.
        /// @throw When no token is found at the current position.
        std::string token(const std::string& delimeters, size_t limit = std::numeric_limits<size_t>::max());

        /// Extracts a line and advances the reader accordinly
        ///
        /// @throw ReaderException::Type::NoLine
        /// @return The extracted line without CR/LF
        std::string line();

        // TODO: Missing tests
        /// Advances the reader from the current position, skipping any empty
        /// lines (CRLF/LF).
        ///
        /// If there are no empty lines at the current
        /// position or the reader has been exhausted, this function has no
        /// effect.
        void trim_empty_lines();

        void consume(size_t amount);

    private:

        /// Returns a constant iterator pointing to the next element in the reader.
        std::vector<uint8_t>::const_iterator    next();

        const std::vector<uint8_t>&             _buffer;
        std::vector<uint8_t>::const_iterator    _head;
};

class ReaderException : public std::exception
{
    public:
        typedef enum Type
        {
            NoLine
        } Type;

        ReaderException(Type type) throw();

        Type    type() const throw();

        virtual const char* what() const throw();

    private:
        Type _type;
};

#ifdef TESTS

class ReaderTests : public Reader
{
    public:
        static void line_empty();
        static void line_basic();
        static void line_one();
        static void line_noline();
    private:
        static std::vector<uint8_t> buffer(const char* content);
};

#endif
