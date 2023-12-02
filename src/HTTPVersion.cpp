#include <cstdlib>
#include "defs.hpp"
#include "HTTPError.hpp"
#include "HTTPVersion.hpp"

using std::string;
using std::ostream;

HTTPVersion::HTTPVersion()
{

}

HTTPVersion::HTTPVersion(unsigned int major, unsigned int minor) throw()
    : _major(major), _minor(minor)
{

}

bool HTTPVersion::is_compatible_with(const HTTPVersion& other) const
{
    return (_major == other._major);
}

HTTPVersion::HTTPVersion(const string& version)
{
    if (version.find("HTTP/") != 0)
        throw HTTPError(Status::BAD_REQUEST);
    if (version.length() != 8)
        throw HTTPError(Status::BAD_REQUEST);
    string major_str = version.substr(5, 1);
    if (!std::isdigit(*major_str.c_str()))
        throw HTTPError(Status::BAD_REQUEST);
    _major = std::strtoul(major_str.c_str(), NULL, 10);
    if (version.at(6) != '.')
        throw HTTPError(Status::BAD_REQUEST);
    string minor_str = version.substr(7, 1);
    if (!std::isdigit(*minor_str.c_str()))
        throw HTTPError(Status::BAD_REQUEST);
    _minor = std::strtoul(minor_str.c_str(), NULL, 10);
}

unsigned int HTTPVersion::major() const
{
    return _major;
}

unsigned int HTTPVersion::minor() const
{
    return _minor;
}

ostream& operator<<(ostream& os, const HTTPVersion& version)
{
    return os << "HTTP/" << version.major() << '.' << version.minor();
}

#ifdef TESTS

void HTTPVersionTests::all()
{
    basic();
    compatible();
}

void HTTPVersionTests::basic()
{
    try {
        HTTPVersion version("HTTP/1.2");
        if (version.major() != 1 || version.minor() != 2)
            throw TESTFAIL;
    }
    catch (...) {
        throw TESTFAIL;
    }
}

void HTTPVersionTests::compatible()
{
    try {
        HTTPVersion one_zero(1, 0);
        HTTPVersion one_one(1, 1);
        HTTPVersion two_zero(2, 0);
        if (!HTTPVersion(1, 0).is_compatible_with(HTTPVersion(1, 1)))
            throw TESTFAIL;
        if (HTTPVersion(2, 0).is_compatible_with(HTTPVersion(1, 0)))
            throw TESTFAIL;
    }
    catch (...) {
        throw TESTFAIL;
    }
}

#endif
