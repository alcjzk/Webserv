#pragma once

#include <string>
#include <ostream>
#include "Reader.hpp"

class HTTPVersion
{
    public:
        HTTPVersion();
        HTTPVersion(const std::string& version);
        HTTPVersion(unsigned int major, unsigned int minor) throw();

        unsigned int    major() const;
        unsigned int    minor() const;

        bool is_compatible_with(const HTTPVersion& other) const;
    private:
        unsigned int _major;
        unsigned int _minor;
};

std::ostream& operator<<(std::ostream& os, const HTTPVersion& version);

#ifdef TESTS

#include "defs.hpp"

class HTTPVersionTests : public HTTPVersion
{
    public:
        static void all();

        static void basic();
        static void compatible();
};

#endif
