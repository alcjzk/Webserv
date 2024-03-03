#pragma once

#include <vector>
#include <string>
#include <chrono>
#include "HostAttributes.hpp"
#include "Status.hpp"

class Config
{
    public:
        using Seconds = std::chrono::seconds;

        Config() = delete;
        Config(const Config& other) = default;
        Config& operator=(const Config& other) = default;
        Config(Config&& other) = default;
        Config& operator=(Config&& other) = default;
        Config(std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root,
               std::pair<std::string, TiniNode*> first_pair);

        const std::string&                 port() const;
        const std::string&                 host() const;
        int                                backlog() const;
        const std::vector<HostAttributes>& attrs() const;
        const HostAttributes&              first_attr() const;
        size_t                             header_buffsize() const;
        std::optional<Path>                error_page(Status status) const;

        /// Returns a timeout in seconds for keeping an inactive connection alive.
        Seconds                            keepalive_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving request headers.
        Seconds                            client_header_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving a request body.
        // TODO: Not used until receive client body is implemented.
        Seconds                            client_body_timeout() const;

        /// Returns a timeout in seconds between successive writes when sending a response back to
        /// the client.
        Seconds                            send_timeout() const;

        /// Returns the maximum time to linger a connection.
        // TODO: Not used until lingering close is implemented.
        Seconds                            lingering_timeout() const;

        /// Returns a timeout in seconds between successive CGI reads.
        // TODO: Not used until CGI is implemented.
        Seconds                            cgi_read_timeout() const;

        /// Returns a timeout in seconds between successive CGI writes.
        // TODO: Not used until CGI is implemented.
        Seconds                            cgi_write_timeout() const;

    private:
        // Timeouts
        Seconds                     _keepalive_timeout = Seconds(75);
        Seconds                     _client_header_timeout = Seconds(60);
        Seconds                     _client_body_timeout = Seconds(60);
        Seconds                     _send_timeout = Seconds(60);
        Seconds                     _lingering_timeout = Seconds(5);
        Seconds                     _cgi_read_timeout = Seconds(60);
        Seconds                     _cgi_write_timeout = Seconds(60);

        std::string                 _port;
        std::string                 _host;
        std::vector<HostAttributes> _attrs;
        HostAttributes              _first_attr;
        int                         _backlog;
        size_t                      _body_size;
        size_t                      _header_buffer_size;
        std::map<int, Path>         _error_pages;
};
