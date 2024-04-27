#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <vector>
#include <utility>
#include <chrono>
#include "Config.hpp"
#include "BasicTask.hpp"
#include "Log.hpp"
#include "Path.hpp"
#include "WriteTask.hpp"

WriteTask::WriteTask(File&& file, std::vector<char>&& content, const Config& config)
    : BasicTask(
          std::move(file), WaitFor::Writable,
          std::chrono::system_clock::now() + config.io_write_timeout()
      ),
      _buffer(std::move(content))
{
}

WriteTask::WriteTask(const Path& path, std::vector<char>&& content, const Config& config)
    : BasicTask(
          File(), WaitFor::Writable, std::chrono::system_clock::now() + config.io_write_timeout()
      ),
      _buffer(std::move(content))
{
    try
    {
        _fd = path.open(O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK | O_CLOEXEC, 0644);
    }
    catch (const std::system_error& error)
    {
        WARN("WriteTask: could not open `" << path << "`: " << error.what());
        throw std::system_error(error);
    }
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

bool WriteTask::is_error() const
{
    return _is_error;
}
