#include <algorithm>
#include <system_error>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "Path.hpp"

using std::ostream;
using std::string;

Path::Path(const string& path)
{
    size_t start = 0;
    size_t end;

    if (path.empty())
        throw std::runtime_error("Empty paths are not yet supported");

    if (path[0] == '/')
    {
        _segments.push_back("/");
        start = 1;
    }

    if (path.size() == 1)
        return ;

    while (true)
    {
        end = path.find_first_of('/', start);
        if (end == string::npos)
        {
            _segments.push_back(path.substr(start));
            break;
        }
        _segments.push_back(path.substr(start, end - start));
        start = end + 1;
    }
}

Path::Path(const char* path) : Path(string(path)) {}

Path::Path(const_iterator first, const_iterator last) : _segments(first, last) {}

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
    string         path;
    const_iterator begin = cbegin();
    const_iterator end = cend();

    if (begin == end)
        return path;
    if (*begin != "/")
        path = *begin;
    std::for_each(begin + 1, end, [&path](const string& segment) { path = path + '/' + segment; });
    return path;
}

Path Path::operator+(const Path& rhs) const
{
    Path path(*this);

    path._segments.reserve(rhs._segments.size());
    path._segments.insert(path.end(), rhs.cbegin(), rhs.cend());
    return path;
}

Path Path::relative(const Path& path, const Path& base)
{
    auto pair = std::mismatch(path.cbegin(), path.cend(), base.cbegin(), base.cend());
    if (pair.second != base.cend())
    {
        throw std::runtime_error("Path is not relative to base");
    }
    if (pair.first == path.cend())
    {
        return Path("/");
    }
    auto result = Path(pair.first, path.cend());
    return result;
}

Path Path::canonical(const Path& path)
{
    // FIXME: Not implemented
    return Path(path);
}

ostream& operator<<(ostream& os, const Path& path)
{
    return os << static_cast<string>(path);
}

Path::Type Path::fetch_type() const
{
    struct stat buffer;
    mode_t      mode;

    if (stat(static_cast<string>(*this).c_str(), &buffer) != 0)
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
