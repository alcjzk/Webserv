#include "CGIReadTask.hpp"
#include "Log.hpp"
#include <signal.h>
#include <sys/wait.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIReadTask::CGIReadTask(
    File&& read_end, const Config& config, pid_t pid
)
    : BasicTask(
          std::move(read_end), WaitFor::Readable,
          std::chrono::system_clock::now() + config.cgi_read_timeout()
      ),
      _pid(pid)
{
    INFO("FD num in read task: " << _fd);
}

// Write body from request to cgi
void CGIReadTask::run()
{
    char buf[4096];
    ssize_t bytes_read = read(_fd, buf, 4096);

    INFO("bytes read: " << bytes_read);
    if (bytes_read < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }

    _buffer.insert(_buffer.end(), buf, buf + bytes_read);
    if (bytes_read < 4096)
    {
        _is_complete = true;
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
    if (_pid)
    {
        INFO("KILLING CHILD IN DESTRUCTOR");
        kill(_pid.value(), SIGKILL);
        auto code = waitpid(_pid.value(), &_exit_status, 0);
    }
}


CGIReadTask::CGIReadTask(CGIReadTask&& moved_from) : BasicTask(
        std::move(moved_from)
        ), _buffer(std::move(moved_from._buffer)), _pid(std::exchange(moved_from._pid, std::nullopt)), _is_error(false)
{
}

CGIReadTask& CGIReadTask::operator=(CGIReadTask&& other)
{
    if (this == &other)
        return *this;
    BasicTask::operator=(std::move(other));
    _buffer = std::move(other._buffer);
    _pid = std::exchange(other._pid, std::nullopt);
    _is_error = false;
    return *this;
}

void CGIReadTask::abort()
{
    INFO("CGIReadTask for fd " << _fd << " timed out.");
    _is_complete = true;
    _is_error = true;
    if (_pid)
    {
        INFO("KILLING CHILD");
        kill(_pid.value(), SIGKILL);
        std::exchange(_pid, std::nullopt);
    }
}
