#include <algorithm>
#include <system_error>
#include <optional>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include "Log.hpp"
#include "Path.hpp"

using std::ostream;
using std::string;

size_t Path::Status::size() const
{
    return st_size;
}

bool Path::Status::is_regular() const
{
    return S_ISREG(st_mode);
}

bool Path::Status::is_directory() const
{
    return S_ISDIR(st_mode);
}

Path::Path(const string& path)
{
    size_t start = 0;
    size_t end;

    if (path[0] == '/')
    {
        _is_root = true;
        start = path.find_first_not_of('/', 1);
    }

    while (start != string::npos)
    {
        end = path.find_first_of('/', start + 1);
        if (end == string::npos)
        {
            _segments.push_back(path.substr(start));
            return;
        }
        _segments.push_back(path.substr(start, end - start));
        start = path.find_first_not_of('/', end + 1);
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

bool Path::is_root() const noexcept
{
    return _is_root;
}

std::optional<Path::Status> Path::status() const noexcept
{
    Status status;

    if (::stat(static_cast<string>(*this).c_str(), &status) != 0)
    {
        WARN("Path::status(): " << strerror(errno));
        return std::nullopt;
    }
    return status;
}

int Path::open(int flags) const
{
    int fd = ::open(static_cast<string>(*this).c_str(), flags);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    return fd;
}

int Path::open(int flags, mode_t mode) const
{
    int fd = ::open(static_cast<string>(*this).c_str(), flags, mode);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    return fd;
}

void Path::is_root(bool value) noexcept
{
    _is_root = value;
}

Path::operator string() const
{
    string         path;
    const_iterator begin = cbegin();
    const_iterator end = cend();

    if (_is_root)
    {
        path = '/';
    }
    if (begin == end)
        return path;
    path += *begin;
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

bool Path::operator==(const Path& rhs) const
{
    if (_is_root != rhs._is_root)
        return false;
    auto pair = std::mismatch(cbegin(), cend(), rhs.cbegin(), rhs.cend());
    if (pair.first != cend() || pair.second != rhs.cend())
        return false;
    return true;
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
    Path result;

    result.is_root(path.is_root());
    for (auto it = path.cbegin(); it != path.cend(); it++)
    {
        if (*it != "." && *it != "..")
        {
            auto next = std::next(it);
            while (next != path.cend() && *next == ".")
            {
                next = std::next(next);
            }
            if (next == path.cend())
            {
                result._segments.push_back(*it);
                return result;
            }
            if (*next != "..")
            {
                result._segments.push_back(*it);
                it = std::next(next);
            }
        }
    }
    return result;
}

ostream& operator<<(ostream& os, const Path& path)
{
    return os << static_cast<string>(path);
}

#ifdef TEST

#include "testutils.hpp"

void PathTest::canonical_test()
{
    BEGIN

    EXPECT((Path::canonical(Path("/foo/../bar")) == Path("/bar")));
    EXPECT((Path::canonical(Path("/.././.././../foo")) == Path("/foo")));
    EXPECT((Path::canonical(Path("foo/bar/././../baz/")) == Path("foo/baz")));
    EXPECT((Path::canonical(Path("foo/bar/././../baz/../.")) == Path("foo/")));

    END
}

void PathTest::repeated_delim_test()
{
    BEGIN

    EXPECT((Path("//////") == Path("/")));
    EXPECT((Path("a////b") == Path("a/b")));

    END
}

#endif
