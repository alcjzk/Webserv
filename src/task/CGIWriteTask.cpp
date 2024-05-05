#include "CGIWriteTask.hpp"
#include "CGIReadTask.hpp"
#include "Log.hpp"
#include <string.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIWriteTask::CGIWriteTask(
    Request&& request, std::vector<char>& post_body, File&& write_end, Config& config
)
    : BasicTask(
          std::move(write_end), WaitFor::Writable,
          std::chrono::system_clock::now() + config.io_write_timeout()
      ),
      _request(request), _post_body(post_body)
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
