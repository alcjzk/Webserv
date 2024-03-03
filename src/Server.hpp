#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include "Config.hpp"
#include "Route.hpp"
#include "Routes.hpp"
#include "HTTPVersion.hpp"
#include "HTTPVersion.hpp"
#include "Routes.hpp"
#include "Request.hpp"
#include "HostAttributes.hpp"

class Server
{
    public:
        ~Server();

        Server() = delete;
        Server(Config&& config);
        Server(const Server&) = delete;
        Server(Server&&) = delete;

        Server&                  operator=(const Server&) = delete;
        Server&                  operator=(Server&&) = delete;

        int                      fd() const;
        const Route*             route(const std::string& uri_path, const std::string& host) const;
        static const HTTPVersion http_version();
        const Config&            config() const;
        const HostAttributes&    map_attributes(std::string host_name) const;

    private:
        Config           _config;
        const char*      _port;
        struct addrinfo* _address_info;
        int              _fd;
        Routes           _routes;
};
