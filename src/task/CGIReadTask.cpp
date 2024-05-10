#include "CGIReadTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "http.hpp"
#include <sys/socket.h>
#include <sys/wait.h>
#include <utility>

// this is for preparing the content to write to the CGI
// assign _pid & __reader.buffer().begin(efdout

CGIReadTask::CGIReadTask(int read_end, const Config& config, Child&& pid)
    : BasicTask(std::move(read_end), WaitFor::Readable), _pid(std::move(pid)),
      _expire_time(config.cgi_read_timeout())
{
    INFO("FD num in read task: " << _fd);
}

void CGIReadTask::run()
{
    INFO("running");
    if (_reader.buffer().unfilled_size() == 0)
    {
        if (!_reader.grow(_upload_limit))
        {
            _is_error = true;
            _is_complete = true;
            return;
        }
    }

    ssize_t bytes_received = recv(
        _fd, _reader.buffer().unfilled(), std::min(_reader.buffer().unfilled_size(), 4096UL), 0
    );
    _reader.buffer().advance(bytes_received);
    INFO(bytes_received);
    if (bytes_received < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _is_error = true;
        _is_complete = true;
        return;
    }
    else if (bytes_received == 0)
    {
        if (_expect == Expect::Headers)
        {
            WARN("CGIReadTask: cgi eof while reading headers");
            _is_error = true;
            _is_complete = true;
            return;
        }
        INFO("CGIReadTask: EOF");
        _pid.wait();
        _response->body(std::vector(_reader.begin(), _reader.end()));
        _is_complete = true;
        return;
    }

    while (!_reader.is_empty() && _expect == Expect::Headers)
        read_headers();
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
    try
    {
        while (auto line = _reader.line())
        {
            if (line->empty())
            {
                if (_reader.is_empty())
                    _reader.buffer().clear();
                else
                {
                    assert(_reader.begin() != _reader.buffer().begin());
                    _reader.buffer().replace(_reader.begin(), _reader.end());
                }
                _reader.rewind();
                _expect = Expect::Body;
                return;
            }
            if (!_response->headers().insert(http::parse_field(line.value())))
            {
                throw std::runtime_error("duplicate field value");
            }
        }
    }
    catch (const std::exception& error)
    {
        WARN("CGIReadTask: " << error.what());
        _is_error = true;
        _is_complete = true;
    }
}
