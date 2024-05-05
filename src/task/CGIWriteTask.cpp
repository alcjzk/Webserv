#include "CGIWriteTask.hpp"
#include "Log.hpp"
#include <string.h>
#include <signal.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIWriteTask::CGIWriteTask(
    Request&& request, const Request::Body& post_body, File&& write_end, pid_t pid, Config& config, int read_end
)
    : BasicTask(
          std::move(write_end), WaitFor::Writable,
          std::chrono::system_clock::now() + config.io_write_timeout()
      ),
      _config(config), _request(request), _read_end(read_end), _post_body(post_body), _pid(std::move(pid))
{}

// Write body from request to cgi
void CGIWriteTask::run()
{
    char*  data = _post_body.data() + _bytes_written_total;
    size_t remainder = _post_body.size() - _bytes_written_total;

    ssize_t bytes_written = write(_fd, data, remainder);
    if (bytes_written < 0)
    {
        WARN("CGIWriteTask: write failed for fd `" << _fd_in << "`");
        _is_error = true;
        _is_complete = true;
        kill(_pid, SIGKILL);
        return;
    }

    _bytes_written_total += size_t(bytes_written);
    if (bytes_written == 0)
        _is_complete = true;
}

void CGIWriteTask::SignalhandlerChild(int sig)
{
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}

int     CGIWriteTask::read_end() const
{
    return _read_end;
}

Config& CGIWriteTask::config()   const
{
    return _config;
}
