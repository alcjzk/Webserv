#include "CGIWriteTask.hpp"
#include "Log.hpp"
#include <string.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIWriteTask::CGIWriteTask(
    Request&& request, const Request::Body& post_body, File&& write_end, Child&& pid,
    const Config& config
)
    : BasicTask(std::move(write_end), WaitFor::Writable), _config(config), _request(request),
      _post_body(post_body), _pid(std::move(pid)), _expire_time(config.cgi_write_timeout())
{
    INFO("Writing post body: \n" << std::string(_post_body.begin(), _post_body.end()));
}

// Write body from request to cgi
void CGIWriteTask::run()
{
    char*  data = _post_body.data() + _bytes_written_total;
    size_t remainder = _post_body.size() - _bytes_written_total;

    ssize_t bytes_written = write(_fd, data, remainder);
    INFO("Write return value: " << bytes_written);
    if (bytes_written < 0)
    {
        WARN("CGIWriteTask: write failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _bytes_written_total += size_t(bytes_written);
    if (bytes_written == 0)
        _is_complete = true;
}

const Config& CGIWriteTask::config() const
{
    return _config;
}

bool CGIWriteTask::is_error() const
{
    return _is_error;
}

void CGIWriteTask::abort()
{
    INFO("CGIWriteTask for fd " << _fd << " timed out.");
    _is_complete = true;
    _is_error = true;
}

std::optional<Task::Seconds> CGIWriteTask::expire_time() const
{
    return _expire_time;
}

Child CGIWriteTask::take_pid() &&
{
    return std::move(_pid);
}

File CGIWriteTask::take_fd() &&
{
    return std::move(_fd);
}
