#include "CGIReadTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include <sys/wait.h>
#include <utility>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIReadTask::CGIReadTask(
    int read_end, const Config& config, Child&& pid
)
    : BasicTask(
          std::move(read_end), WaitFor::Readable
      ),
      _pid(std::move(pid)), _expire_time(config.cgi_read_timeout())
{
    INFO("FD num in read task: " << _fd);
}

// Write body from request to cgi
void CGIReadTask::run()
{
    char buf[4096];
    ssize_t bytes_read = read(_fd, buf, 4096);

    INFO("READING")
    if (bytes_read < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _buffer.insert(_buffer.end(), buf, buf + bytes_read);
    if (bytes_read == 0)
    {
        _pid.wait();
        _is_complete = true;
        return;
    }
    if (_buffer.size() > _upload_limit)
    {
        INFO("Buffer size limit exceeded");
        _is_complete = true;
        _is_error = true;
        return;
    }
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

CGIReadTask::~CGIReadTask() {
    INFO("Destructing read task")
}

void CGIReadTask::abort()
{
    // BREAK
    INFO("Aborting read task");
    _is_complete = true;
    _is_error = true;
}

std::optional<Task::Seconds> CGIReadTask::expire_time() const
{
    return _expire_time;
}
