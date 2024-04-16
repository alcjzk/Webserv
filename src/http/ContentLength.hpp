#pragma once

#include <string>
#include <cstddef>

/// Typesafe Content-Length value.
class ContentLength
{
    public:
        /// Constructs the type from a valid value.
        ContentLength(size_t value) noexcept;

        /// Constructs the type from a string.
        ///
        /// @throws
        /// BAD_REQUEST - value is invalid or empty.
        /// CONTENT_TOO_LARGE - value is too large.
        ContentLength(const std::string& value);

        /// Implicitly converts to size_t.
        operator size_t() noexcept;

    private:
        size_t _value;
};

#ifdef TEST

class ContentLengthTest : public ContentLength
{
    public:
        static void string_constructor_basic_test();
        static void string_constructor_invalid_test();
};

#endif
