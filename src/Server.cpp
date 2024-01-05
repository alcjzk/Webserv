#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <stdexcept>
#include <cassert>
#include "Reader.hpp"
#include "Error.hpp"
#include "RequestLine.hpp"
#include "HTTPError.hpp"
#include "Log.hpp"
#include "Server.hpp"

using std::string;

Server::Server(const Config &config) : _config(config), _port(config.ports().front().c_str()), _fd(-1)
{
    struct addrinfo hints;
    int             status;

    hints = (struct addrinfo){};
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

int Server::fd()
{
    return _fd;
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
            return ;
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
    catch (...) { assert(false); }
    _is_complete = true;
    (void)close(_fd);
}

ServerReceiveRequestTask::ServerReceiveRequestTask(int fd) :
    Task(fd, Readable),
    _expect(REQUEST_LINE),
    _bytes_received_total(0),
    _buffer(_header_buffer_size),
    _reader(_buffer),
    _is_partial_data(true)
{

}

size_t ServerReceiveRequestTask::buffer_size_available()
{
    return _header_buffer_size - _bytes_received_total;
}

char* ServerReceiveRequestTask::buffer_head()
{
    return _buffer.data() + _bytes_received_total;
}

void ServerReceiveRequestTask::fill_buffer()
{
    ssize_t                 bytes_received = 0;

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
        _request._request_line = RequestLine(_reader.line());
        if (!_request.http_version().is_compatible_with(Server::http_version()))
        {
            throw HTTPError(Status::HTTP_VERSION_NOT_SUPPORTED);
        }
        INFO(_request._request_line);
        _expect = HEADERS;
    }
    catch (const ReaderException& error)
    {
        // No line in buffer, we can just try to read again.
        assert(error.type() == ReaderException::NoLine);
        _is_partial_data = true;
    }
}

void ServerReceiveRequestTask::receive_headers()
{
    try
    {
        while (true)
        {
            string line = _reader.line(); // TODO: Probably unnecessary copy assingnment
            if (line.empty())
            {
                // End of headers
                INFO("End of headers");
                Response* response = _request.into_response();
                Runtime::enqueue(new ServerSendResponseTask(_fd, response));
                _is_complete = true;
                return ;
            }
            // Convert line to header
        }
    }
    catch (const ReaderException& error)
    {
        // No line in buffer, we can just try to read again.
        assert(error.type() == ReaderException::NoLine);
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
                    continue ;
                case HEADERS:
                    receive_headers();
                    continue ;
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
        Response* response = new TextResponse(string(error.what()) + '\n');
        Runtime::enqueue(new ServerSendResponseTask(_fd, response));
        _is_complete = true;
    }
    catch (const std::exception& error)
    {
        ERR(error.what());
        close(_fd);
        _is_complete = true;
    }
}

ServerReceiveRequestTask::~ServerReceiveRequestTask()
{

}

ServerAcceptTask::ServerAcceptTask(Server& server)
    : Task(server.fd(), Readable), _server(server)
{

}

void ServerAcceptTask::run()
{
    int         fd;

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
        Runtime::enqueue(new ServerReceiveRequestTask(fd));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}

ServerAcceptTask::~ServerAcceptTask()
{

}
