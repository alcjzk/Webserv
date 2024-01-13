#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
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

Path::Type Path::type()
{
    if (_type == NONE)
    {
        _type = fetch_type();
    }
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

Path::Type Path::fetch_type() const
{
    struct stat buffer;
    mode_t      mode;

    if (stat("", &buffer) != 0)
    {
        if (errno != ENOENT && errno != ENOTDIR)
        {
            throw std::system_error(errno, std::system_category());
        }
        return NOT_FOUND;
    }

    mode = buffer.st_mode;
    if (S_ISREG(mode))
        return REGULAR;
    if (S_ISDIR(mode))
        return DIRECTORY;
    if (S_ISCHR(mode))
        return CHARACTER;
    if (S_ISBLK(mode))
        return BLOCK;
    if (S_ISFIFO(mode))
        return FIFO;
    if (S_ISLNK(mode))
        return LINK;
    if (S_ISSOCK(mode))
        return SOCKET;

    return UNKNOWN;
}
