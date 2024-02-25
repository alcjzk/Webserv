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
#include "TimeoutResponse.hpp"
#include "ServerAcceptTask.hpp"

using std::optional;
using std::string;
using std::vector;

Server::Server(const Config& config)
    : _config(config), _port(config.ports().front().c_str()), _fd(-1)
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
    try
    {
        _routes.push(Route("/", "www"));
    }
    catch (const std::runtime_error& error)
    {
        WARN(error.what());
    }
    INFO("Listening on " << _fd);
    Runtime::enqueue(new ServerAcceptTask(*this));
}

const HTTPVersion Server::http_version()
{
    return HTTPVersion(1, 1);
}

const Config& Server::config() const
{
    return _config;
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

const Route* Server::route(const std::string& uri_path) const
{
    return _routes.find(uri_path);
}
