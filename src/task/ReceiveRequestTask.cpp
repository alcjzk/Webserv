#include <string.h>
#include <unistd.h>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <string>
#include <optional>
#include <exception>
#include <errno.h>
#include <sys/socket.h>
#include "Status.hpp"
#include "Response.hpp"
#include "Reader.hpp"
#include "Request.hpp"
#include "ReceiveRequestTask.hpp"
#include "ErrorResponseTask.hpp"
#include "SendResponseTask.hpp"
#include "TimeoutResponse.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Error.hpp"
#include "Log.hpp"
#include <utility>
#include "BasicTask.hpp"
#include "Server.hpp"
#include "File.hpp"
#include "Task.hpp"

using std::string;
using std::vector;

ReceiveRequestTask::ReceiveRequestTask(const Server& server, File&& file)
    : BasicTask(
          std::move(file), WaitFor::Readable,
          std::chrono::system_clock::now() + server.config().keepalive_timeout()
      ),
      _expect(REQUEST_LINE), _bytes_received_total(0), _reader(vector<char>(_header_buffer_size)),
      _is_partial_data(true), _server(server)
{
}

size_t ReceiveRequestTask::buffer_size_available()
{
    return _header_buffer_size - _bytes_received_total;
}

char* ReceiveRequestTask::buffer_head()
{
    return _reader.data() + _bytes_received_total;
}

void ReceiveRequestTask::fill_buffer()
{
    ssize_t bytes_received = 0;

    bytes_received = recv(_fd, buffer_head(), buffer_size_available(), 0);
    if (bytes_received == 0)
    {
        if (buffer_size_available() <= 0)
        {
            // Out of buffer
            throw HTTPError(Status::BAD_REQUEST);
        }
        throw Error(Error::CLOSED);
    }
    else if (bytes_received == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    _bytes_received_total += bytes_received;
    _is_partial_data = false;
}

void ReceiveRequestTask::receive_start_line()
{
    try
    {
        _reader.trim_empty_lines();
        _builder->request_line(_reader.line(RequestLine::MAX_LENGTH).value());
        INFO(_builder->_request_line);
        _expect = HEADERS;
        _expire_time = std::chrono::system_clock::now() + _server.config().client_header_timeout();
    }
    catch (const std::bad_optional_access&)
    {
        _is_partial_data = true;
    }
    catch (const Reader::LineLimitError&)
    {
        throw HTTPError(Status::URI_TOO_LONG);
    }
}

void ReceiveRequestTask::receive_headers()
{
    string    line;
    Response* response;

    try
    {
        while (true)
        {
            line = _reader.line().value();
            if (line.empty())
            {
                INFO("End of headers");
                Request request = std::exchange(_builder, std::nullopt).value().build();
                Runtime::enqueue(request.process(_server, std::move(_fd)));
                _is_complete = true;
                return;
            }
            _builder->header(line);
            INFO(_builder->_headers.back());
        }
    }
    catch (const std::bad_optional_access&)
    {
        _is_partial_data = true;
    }
}

void ReceiveRequestTask::run()
{
    try
    {
        if (_is_partial_data)
        {
            fill_buffer();
        }
        while (!_is_partial_data)
        {
            switch (_expect)
            {
                case REQUEST_LINE:
                    receive_start_line();
                    continue;
                case HEADERS:
                    receive_headers();
                    continue;
                default:
                    assert(false);
            }
        }
    }
    catch (const Error& error)
    {
        assert(error == Error::CLOSED);
        WARN(error.what());
        disable_linger();
        _is_complete = true;
    }
    catch (const HTTPError& error)
    {
        WARN(error.what());
        Runtime::enqueue(new ErrorResponseTask(std::move(_fd), _server, error.status()));
        _is_complete = true;
    }
    catch (const std::exception& error)
    {
        ERR(error.what());
        _is_complete = true;
    }
}

void ReceiveRequestTask::abort()
{
    INFO("ReceiveRequestTask for fd " << _fd << " timed out");
    _is_complete = true;
    Runtime::enqueue(new SendResponseTask(_server, std::move(_fd), new TimeoutResponse()));
}

void ReceiveRequestTask::disable_linger()
{
    struct linger linger;

    linger.l_onoff = 1;
    linger.l_linger = 0;
    if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) == -1)
        WARN("failed to disable linger for fd `" << _fd << "`");
}
