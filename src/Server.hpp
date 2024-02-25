#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include "Config.hpp"
#include "Route.hpp"
#include "Routes.hpp"
#include "HTTPVersion.hpp"

class Server
{
    public:
        ~Server();

        Server(const Config& config);
        Server(const Server&) = delete;
        Server(Server&&) = delete;

        Server&                  operator=(const Server&) = delete;
        Server&                  operator=(Server&&) = delete;

        int                      fd() const;
        const Route*             route(const std::string& uri_path) const;
        static const HTTPVersion http_version();
        const Config&            config() const;

    private:
        const Config&    _config;
        const char*      _port;
        struct addrinfo* _address_info;
        int              _fd;
        Routes           _routes;
};
