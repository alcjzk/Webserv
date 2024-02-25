#include <string.h>
#include <unistd.h>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <string>
#include "ServerReceiveRequestTask.hpp"
#include "ServerSendResponseTask.hpp"
#include "TimeoutResponse.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Error.hpp"
#include "Log.hpp"
#include "http.hpp"

using std::string;
using std::vector;

ServerReceiveRequestTask::ServerReceiveRequestTask(const Server& server, int fd)
    : Task(fd, Readable, std::chrono::system_clock::now() + server.config().keepalive_timeout()),
      _expect(REQUEST_LINE), _bytes_received_total(0), _reader(vector<char>(_header_buffer_size)),
      _is_partial_data(true), _server(server)
{
}

size_t ServerReceiveRequestTask::buffer_size_available()
{
    return _header_buffer_size - _bytes_received_total;
}

char* ServerReceiveRequestTask::buffer_head()
{
    return _reader.data() + _bytes_received_total;
}

void ServerReceiveRequestTask::fill_buffer()
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

void ServerReceiveRequestTask::receive_start_line()
{
    try
    {
        _reader.trim_empty_lines();
        _request._request_line = RequestLine(_reader.line().value());
        if (!_request.http_version().is_compatible_with(Server::http_version()))
        {
            throw HTTPError(Status::HTTP_VERSION_NOT_SUPPORTED);
        }
        INFO(_request._request_line);
        _expect = HEADERS;
        _expire_time = std::chrono::system_clock::now() + _server.config().client_header_timeout();
    }
    catch (const std::bad_optional_access&)
    {
        _is_partial_data = true;
    }
}

void ServerReceiveRequestTask::receive_headers()
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
                response = _request.into_response(_server);
                Runtime::enqueue(new ServerSendResponseTask(_server.config(), _fd, response));
                _is_complete = true;
                return;
            }
            // Append to pre-existing headers when header is prefixed by SP/HT
            if ((line[0] == http::SP || line[0] == http::HT) && !_request._headers.empty())
            {
                _request._headers.back().append(line);
            }
            else
            {
                _request._headers.push_back(Header(line));
            }
            INFO(_request._headers.back());
        }
    }
    catch (const std::bad_optional_access&)
    {
        _is_partial_data = true;
    }
}

void ServerReceiveRequestTask::run()
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
        close(_fd);
        _is_complete = true;
    }
    catch (const HTTPError& error)
    {
        WARN(error.what());
        // TODO: Replace with proper error response
        Runtime::enqueue(
            new ServerSendResponseTask(_server.config(), _fd, new Response(error.status())));
        _is_complete = true;
    }
    catch (const std::exception& error)
    {
        ERR(error.what());
        close(_fd);
        _is_complete = true;
    }
}

void ServerReceiveRequestTask::abort()
{
    INFO("ReceiveRequestTask for fd " << _fd << " timed out");
    _is_complete = true;
    Runtime::enqueue(new ServerSendResponseTask(_server.config(), _fd, new TimeoutResponse()));
}

ServerReceiveRequestTask::~ServerReceiveRequestTask() {}
