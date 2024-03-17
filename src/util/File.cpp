#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <utility>
#include "Log.hpp"
#include "File.hpp"

using std::ostream;

File::~File()
{
    this->close();
}

File::File(int fd) noexcept : _fd(fd) {}

File::File(File&& other) noexcept : _fd(std::exchange(other._fd, std::nullopt)) {}

File& File::operator=(File&& other) noexcept
{
    this->close();
    _fd = std::exchange(other._fd, std::nullopt);
    return *this;
}

File::operator int() const
{
    if (_fd.has_value())
        return *_fd;
    return -1;
}

int File::fd() const
{
    return *this;
}

int File::take_fd()
{
    return std::exchange(_fd, std::nullopt).value();
}

void File::close()
{
    if (_fd)
    {
        if (::close(*_fd) != 0)
        {
            WARN("fd '" << *_fd << "' did not close properly: " << strerror(errno));
        }
        INFO("closed fd '" << *_fd << "'");
    }
}

ostream& operator<<(ostream& os, const File& file)
{
    if (!file._fd.has_value())
        return os << "closed fd";
    return os << file.fd();
}
