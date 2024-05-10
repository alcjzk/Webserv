#include <sys/types.h>
#include <utility>
#include <unistd.h>
#include <cstddef>
#include "Log.hpp"
#include "File.hpp"
#include "Task.hpp"
#include "Config.hpp"
#include "BasicTask.hpp"
#include "ReadTask.hpp"

using std::optional;
using std::vector;
using Seconds = Task::Seconds;

ReadTask::ReadTask(File&& file, size_t size, const Config& config)
    : BasicTask(std::move(file), WaitFor::Readable), _size(size), _buffer(size, 0),
      _expire_time(config.io_read_timeout())
{
}

vector<char> ReadTask::buffer() &&
{
    return std::move(_buffer);
}

void ReadTask::run()
{
    char*   data = _buffer.data() + _bytes_read_total;
    size_t  remainder = _size - _bytes_read_total;
    ssize_t bytes_read = read(_fd, data, std::min(4096UL, remainder));

    if (bytes_read < 0)
    {
        WARN("ReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _bytes_read_total += bytes_read;
    if (_bytes_read_total == _size || bytes_read == 0)
        _is_complete = true;
}

optional<Seconds> ReadTask::expire_time() const
{
    return _expire_time;
}

bool ReadTask::is_error() const
{
    return _is_error;
}
