#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <vector>
#include <utility>
#include <errno.h>
#include <cstring>
#include "Config.hpp"
#include "BasicTask.hpp"
#include "Log.hpp"
#include "Path.hpp"
#include "WriteTask.hpp"

using std::optional;
using Seconds = Task::Seconds;

WriteTask::WriteTask(File&& file, std::vector<char>&& content, const Config& config)
    : BasicTask(std::move(file), WaitFor::Writable), _buffer(std::move(content)),
      _expire_time(config.io_write_timeout())
{
}

WriteTask::WriteTask(const Path& path, std::vector<char>&& content, const Config& config)
    : BasicTask(File(), WaitFor::Writable), _buffer(std::move(content)),
      _expire_time(config.io_write_timeout())
{
    auto fd = path.open(O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK | O_CLOEXEC, 0644);
    if (!fd)
    {
        WARN("WriteTask: could not open `" << path << "`: " << strerror(errno));
        throw std::system_error(errno, std::system_category());
    }
    _fd = fd.value();
}

void WriteTask::run()
{
    char*  data = _buffer.data() + _bytes_written_total;
    size_t remainder = _buffer.size() - _bytes_written_total;

    ssize_t bytes_written = write(_fd, data, remainder);
    if (bytes_written < 0)
    {
        WARN("WriteTask: write failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _bytes_written_total += size_t(bytes_written);
    if (bytes_written == 0)
        _is_complete = true;
}

optional<Seconds> WriteTask::expire_time() const
{
    return _expire_time;
}

bool WriteTask::is_error() const
{
    return _is_error;
}
