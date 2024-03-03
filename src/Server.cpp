#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdexcept>
#include <algorithm>
#include "Runtime.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "ServerAcceptTask.hpp"

using std::string;

Server::Server(Config&& config) : _config(std::move(config)), _port(_config.port().c_str()), _fd(-1)
{
    struct addrinfo hints;
    int             status;
    int             sockopt_value = 1;

    hints = {};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(_config.host().c_str(), _port, &hints, &_address_info);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    _fd = socket(_address_info->ai_family, _address_info->ai_socktype, _address_info->ai_protocol);
    if (_fd == -1)
        throw std::runtime_error(strerror(errno));
    if (fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt_value, sizeof(sockopt_value)) == -1)
    {
        ERR("Server: setsockopt: failure");
    }
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

const HostAttributes& Server::map_attributes(std::string host_name) const
{
    const std::vector<HostAttributes>& attributes = _config.attrs();
    const auto&                        attr =
        std::find_if(attributes.begin(), attributes.end(),
                     [host_name](const auto& a) { return a.hostname() == host_name; });
    if (attr == attributes.end())
        return attributes.front();
    return *attr;
}

const Route* Server::route(const std::string& uri_path, const std::string& host) const
{
    const std::vector<HostAttributes>& attributes = _config.attrs();
    const auto                         attr =
        std::find_if(attributes.begin(), attributes.end(),
                     [host](const HostAttributes& a) { return (a.hostname() == host); });
    if (attr == attributes.end())
    {
        INFO("Attribute not found returning " << _config.first_attr().hostname());
        return (_config.first_attr()).routes().find(uri_path);
    }
    return ((*attr).routes().find(uri_path));
}
