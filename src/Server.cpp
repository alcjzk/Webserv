#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <stdexcept>
#include <cassert>
#include <algorithm>
#include "Reader.hpp"
#include "Error.hpp"
#include "RequestLine.hpp"
#include "HTTPError.hpp"
#include "Log.hpp"
#include "http.hpp"
#include "Server.hpp"

using std::string;
using std::vector;

Server::Server(const Config& config)
    : _config(config), _port(config.port().c_str()), _fd(-1), _attributes(config.attrs())
{
    struct addrinfo hints;
    int             status;
    int             sockopt_value = 1;

    hints = {};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, _port, &hints, &_address_info);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    _fd = socket(_address_info->ai_family, _address_info->ai_socktype, _address_info->ai_protocol);
    if (_fd == -1)
        throw std::runtime_error(strerror(errno));
    if (fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt_value,
               sizeof(sockopt_value)); // TODO: Could warn on error here
    if (bind(_fd, _address_info->ai_addr, _address_info->ai_addrlen) == -1)
        throw std::runtime_error(strerror(errno));
    if (listen(_fd, _config.backlog()) == -1)
        throw std::runtime_error(strerror(errno));
    INFO("Listening on " << _fd);
    Runtime::enqueue(new ServerAcceptTask(*this));
}

const HTTPVersion Server::http_version()
{
    return HTTPVersion(1, 1);
}

Server::~Server()
{
    if (_fd != -1)
        (void)close(_fd);
    freeaddrinfo(_address_info);
    INFO("Graceful exit");
}

int Server::fd() const
{
    return _fd;
}

const Config& Server::config() const
{
    return _config;
}

ServerSendResponseTask::ServerSendResponseTask(int fd, Response* response)
    : Task(fd, Writable), _response(response)
{
}

ServerSendResponseTask::~ServerSendResponseTask()
{
    delete _response;
}

void ServerSendResponseTask::run()
{
    try
    {
        if (!_response->send(_fd))
            return;
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
    catch (...)
    {
        assert(false);
    }
    _is_complete = true;
    (void)close(_fd);
}

const Route* Server::route(const std::string& uri_path, const std::string& host) const
{
    const auto attr = std::find_if(_attributes.begin(), _attributes.end(),
                                   [host](const HostAttributes& a) { return (a.hostname() == host); });
    if (attr == _attributes.end())
        return (*_attributes.begin()).routes().find(uri_path);
    return ((*attr).routes().find(uri_path));
}

ServerReceiveRequestTask::ServerReceiveRequestTask(const Server& server, int fd)
    : Task(fd, Readable), _expect(REQUEST_LINE), _bytes_received_total(0),
      _reader(vector<char>(server.config().header_buffsize())), _is_partial_data(true), _server(server)
{
}

size_t ServerReceiveRequestTask::buffer_size_available()
{
    return _server.config().header_buffsize() - _bytes_received_total;
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
                Runtime::enqueue(new ServerSendResponseTask(_fd, response));
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
        Runtime::enqueue(new ServerSendResponseTask(_fd, new Response(error.status())));
        _is_complete = true;
    }
    catch (const std::exception& error)
    {
        ERR(error.what());
        close(_fd);
        _is_complete = true;
    }
}

ServerReceiveRequestTask::~ServerReceiveRequestTask() {}

ServerAcceptTask::ServerAcceptTask(const Server& server)
    : Task(server.fd(), Readable), _server(server)
{
}

void ServerAcceptTask::run()
{
    int fd;

    try
    {
        fd = accept(_server.fd(), NULL, NULL);
        if (fd == -1)
        {
            throw std::runtime_error(strerror(errno));
        }
        if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        {
            (void)close(fd);
            throw std::runtime_error(strerror(errno));
        }
        INFO("Client connected on fd " << fd);
        Runtime::enqueue(new ServerReceiveRequestTask(_server, fd));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}

ServerAcceptTask::~ServerAcceptTask() {}
