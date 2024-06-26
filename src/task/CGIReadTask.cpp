#include <sys/socket.h>
#include <sys/wait.h>
#include <utility>
#include <cassert>
#include "CGIReadTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "http.hpp"

CGIReadTask::CGIReadTask(File&& read_end, const Config& config, Child&& pid)
    : BasicTask(std::move(read_end), WaitFor::Readable), _pid(std::move(pid)),
      _expire_time(config.cgi_read_timeout())
{
}

void CGIReadTask::run()
{
    if (_reader.buffer().unfilled_size() == 0)
    {
        if (!_reader.grow(MAX_CONTENT))
        {
            _error_status = Status::INTERNAL_SERVER_ERROR;
            _is_complete = true;
            return;
        }
    }

    ssize_t bytes_received = recv(
        _fd, _reader.buffer().unfilled(), std::min(_reader.buffer().unfilled_size(), 4096UL), 0
    );
    if (bytes_received < 0)
    {
        WARN("CGIReadTask: read failed for fd `" << _fd << "`");
        _error_status = Status::INTERNAL_SERVER_ERROR;
        _is_complete = true;
        return;
    }
    else if (bytes_received == 0)
    {
        if (_expect == Expect::Headers)
        {
            WARN("CGIReadTask: cgi eof while reading headers");
            _error_status = Status::BAD_GATEWAY;
            _is_complete = true;
            return;
        }
        _pid.wait();
        _response->body(std::vector(_reader.begin(), _reader.end()));
        _is_complete = true;
        return;
    }
    _reader.buffer().advance(bytes_received);

    while (!_reader.is_empty() && _expect == Expect::Headers)
        read_headers();
}

int CGIReadTask::error() const
{
    return (_error_status);
}

std::unique_ptr<Response> CGIReadTask::response() &&
{
    return std::move(_response);
}

void CGIReadTask::abort()
{
    WARN("CGIReadTask: timed out");
    _is_complete = true;
    _error_status = Status::GATEWAY_TIMEOUT;
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
            auto header = http::parse_field(line.value());

            if (!_response->headers().insert(header))
            {
                auto previous = _response->headers().find(header.first);
                previous->second.append(header.second);
            }
        }
    }
    catch (const std::exception& error)
    {
        WARN("CGIReadTask: " << error.what());
        _error_status = Status::INTERNAL_SERVER_ERROR;
        _is_complete = true;
    }
}
