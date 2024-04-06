#pragma once

#include <string>
#include <ostream>

class HTTPVersion
{
    public:
        static constexpr const char* const PREFIX = "HTTP/";
        static const char                  DELIMITER = '.';
        static const size_t                MAX_LENGTH = 8;

        HTTPVersion() = default;
        HTTPVersion(const std::string& version);
        constexpr HTTPVersion(unsigned int major, unsigned int minor) : _major(major), _minor(minor)
        {
        }

        unsigned int major() const;
        unsigned int minor() const;

        bool         is_compatible_with(const HTTPVersion& other) const;
        std::string  to_string() const;

    private:
        unsigned int _major;
        unsigned int _minor;
};

std::ostream& operator<<(std::ostream& os, const HTTPVersion& version);

#ifdef TESTS

class HTTPVersionTests : public HTTPVersion
{
    public:
        static void all();

        static void basic();
        static void compatible();
};

#endif