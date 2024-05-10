#include "CGIReadTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include <sys/socket.h>
#include <sys/wait.h>
#include <utility>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIReadTask::CGIReadTask(int read_end, const Config& config, Child&& pid)
    : BasicTask(std::move(read_end), WaitFor::Readable), _pid(std::move(pid)),
      _expire_time(config.cgi_read_timeout())
{
    INFO("FD num in read task: " << _fd);
}

void CGIReadTask::run()
{
    if (_reader.buffer().unfilled_size() == 0)
    {
        if (!_reader.grow(_upload_limit))
        {
            _is_error = true;
            _is_complete = true;
            return;
        }
    }

    ssize_t bytes_received = recv(_fd, _reader.buffer().unfilled(), 4096UL, 0);
    if (bytes_received < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }
    else if (bytes_received == 0)
    {
        INFO("CGIReadTask: EOF");
        _pid.wait();
        _response->body(std::vector(_reader.begin(), _reader.end()));
        _is_complete = true;
        return;
    }

    switch (_expect)
    {
        case Expect::Headers:
            read_headers();
            break;
        case Expect::Body:
        default:
            break;
    }
}

bool CGIReadTask::is_error() const
{
    return (_is_error);
}

std::unique_ptr<Response> CGIReadTask::response() &&
{
    return std::move(_response);
}

CGIReadTask::~CGIReadTask()
{
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

void CGIReadTask::read_headers()
{

}
