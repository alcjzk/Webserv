#include "CGIReadTask.hpp"
#include "Log.hpp"
#include <string.h>
#include <signal.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIReadTask::CGIReadTask(
    File&& read_end, Config& config, pid_t pid
)
    : BasicTask(
          std::move(read_end), WaitFor::Readable,
          std::chrono::system_clock::now() + config.io_write_timeout()
      ),
      _pid(pid)
{
}

// Write body from request to cgi
void CGIReadTask::run()
{
    char*   data = _buffer.data() + _bytes_read_total;
    size_t  remainder = _size - _bytes_read_total;
    ssize_t bytes_read = read(_fd, data, remainder);

    if (bytes_read < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _bytes_read_total += bytes_read;
    if (_bytes_read_total == _size || bytes_read == 0)
        _is_complete = true;
}

void CGIReadTask::SignalhandlerChild(int sig)
{
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}

bool CGIReadTask::is_error() const
{
    return (_is_error);
}

std::vector<char>&& CGIReadTask::buffer()
{
    return (std::move(_buffer));
}

CGIReadTask::~CGIReadTask() { kill(_pid, SIGKILL); }
