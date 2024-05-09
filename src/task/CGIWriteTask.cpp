#include "CGIWriteTask.hpp"
#include "Log.hpp"
#include <string.h>
#include <signal.h>

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIWriteTask::CGIWriteTask(
    Request&& request, const Request::Body& post_body, int write_end, pid_t pid, Config& config
)
    : BasicTask(
          File(), WaitFor::Writable
      ),
      _config(config), _request(request), _write_end(write_end), _post_body(post_body),
      _pid(std::move(pid)), _expire_time(config.cgi_write_timeout())
{
    INFO("Writing post body: \n" << std::string(_post_body.begin(), _post_body.end()));
}

// Write body from request to cgi
void CGIWriteTask::run()
{
    char*  data = _post_body.data() + _bytes_written_total;
    size_t remainder = _post_body.size() - _bytes_written_total;

    ssize_t bytes_written = write(_write_end, data, remainder);
    INFO("Write return value: " << bytes_written);
    if (bytes_written < 0)
    {
        WARN("CGIWriteTask: write failed for fd `" << _write_end << "`");
        _is_error = true;
        _is_complete = true;
        if (_pid)
            kill(_pid.value(), SIGKILL);
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

const Config& CGIWriteTask::config() const
{
    return _config;
}

bool CGIWriteTask::is_error() const
{
    return _is_error;
}

int CGIWriteTask::write_end() const
{
    return _write_end;
}

CGIWriteTask::~CGIWriteTask()
{
    INFO("WRITE TASK DESTRUCTOR")
    // if (_pid)
    // {
    //     INFO("KILLING CHILD IN DESTRUCTOR");
    //     kill(_pid.value(), SIGKILL);
    //     auto code = waitpid(_pid.value(), &_exit_status, 0);
    // }
}

void CGIWriteTask::abort()
{
    INFO("CGIWriteTask for fd " << _write_end << " timed out.");
    _is_complete = true;
    _is_error = true;
    if (_pid)
    {
        INFO("KILLING CHILD");
        kill(_pid.value(), SIGKILL);
        std::exchange(_pid, std::nullopt);
    }
}

int CGIWriteTask::fd() const
{
    return _write_end;
}

CGIWriteTask::CGIWriteTask(CGIWriteTask&& moved_from)
    : BasicTask(std::move(moved_from)), _config(moved_from._config),
      _request(std::move(moved_from._request)), _write_end(moved_from._write_end), _post_body(std::move(moved_from._post_body)),
      _environment(std::move(moved_from._environment)),
      _bytes_written_total(moved_from._bytes_written_total),
      _pid(std::exchange(moved_from._pid, std::nullopt)), _is_error(false), _exit_status(0)
{
    INFO("Move constructor for writetask");
}

CGIWriteTask& CGIWriteTask::operator=(CGIWriteTask&& other)
{
    INFO("Move assignment operator for writetask");
    if (this == &other)
        return *this;
    BasicTask::operator=(std::move(other));
    _pid = std::exchange(other._pid, std::nullopt);
    _is_error = false;
    _config = std::move(other._config);
    _request = std::move(other._request);
    _post_body = std::move(other._post_body);
    _environment = std::move(other._environment);
    _bytes_written_total = other._bytes_written_total;
    _pid = std::exchange(other._pid, std::nullopt);
    _is_error = other._is_error;
    _write_end= other._write_end;
    _exit_status = other._exit_status;
    return *this;
}


std::optional<Task::Seconds> CGIWriteTask::expire_time() const
{
    return _expire_time;
}

void CGIWriteTask::terminate(bool err)
{
    INFO("TERMINATED CHILD");
    _is_complete = true;
    if (err)
        _is_error = true;
}
