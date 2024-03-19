#include <string.h>
#include <unistd.h>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <string>
#include "ReceiveRequestTask.hpp"
#include "SendResponseTask.hpp"
#include "TimeoutResponse.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Error.hpp"
#include "Log.hpp"
#include "ErrorResponse.hpp"

using std::string;
using std::vector;

ReceiveRequestTask::ReceiveRequestTask(const Server& server, File&& file)
    : Task(std::move(file), Readable,
           std::chrono::system_clock::now() + server.config().keepalive_timeout()),
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
        // Client closed the connection
        assert(error == Error::CLOSED);
        WARN(error.what());
        _is_complete = true;
    }
    catch (const HTTPError& error)
    {
        std::optional<Path> error_path = _server.config().error_page(error.status());

        WARN(error.what());

        Response* response;
        if (error_path.has_value())
            response = new ErrorResponse(error_path.value(), error.status());
        else
            response = new ErrorResponse(_server.config().error_str(), error.status());

        Runtime::enqueue(new SendResponseTask(_server, std::move(_fd), response));
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
