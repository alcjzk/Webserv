#include "Path.hpp"

using std::ostream;
using std::string;

Path::Path(const string& path)
{
    // TODO:
}

Path::Path(const char* path)
{
    // TODO:
}

Path::iterator Path::begin() noexcept
{
    return _segments.begin();
}

Path::iterator Path::end() noexcept
{
    return _segments.end();
}

Path::const_iterator Path::cbegin() const noexcept
{
    return _segments.cbegin();
}

Path::const_iterator Path::cend() const noexcept
{
    return _segments.cend();
}

Path::Type Path::type() const
{
    // TODO:
    return _type;
}

Path::operator string() const
{
    // TODO:
    return string();
}

Path Path::operator+(const Path& rhs) const
{
    // TODO:
    return Path();
}

Path Path::relative(const Path& path, const Path& base)
{
    // TODO:
    return Path();
}

Path Path::canonical(const Path& path)
{
    // TODO:
    return Path();
}

ostream& operator<<(ostream& os, const Path& path)
{
    // TODO:
    return os;
}
