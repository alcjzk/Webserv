#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <optional>

class Config
{
    public:
        using Seconds = std::chrono::seconds;

        Config();

        const std::vector<std::string>& ports() const;
        int                             backlog() const;

        /// Returns an optional timeout in seconds for keeping an inactive connection alive.
        std::optional<Seconds>          keepalive_timeout() const;

        /// Returns an optional timeout in seconds between successive reads when receiving request
        /// headers.
        std::optional<Seconds>          client_header_timeout() const;

        /// Returns an optional timeout in seconds between successive reads when receiving a
        /// request body.
        // TODO: Not used until receive client body is implemented.
        std::optional<Seconds>          client_body_timeout() const;

        /// Returns an optional timeout in seconds between successive writes when sending a
        /// response back to the client.
        std::optional<Seconds>          send_timeout() const;

        /// Returns the maximum time to linger a connection.
        // TODO: Not used until lingering close is implemented.
        Seconds                         lingering_timeout() const;

        /// Returns an optional timeout in seconds between successive CGI reads.
        // TODO: Not used until CGI is implemented.
        std::optional<Seconds>          cgi_read_timeout() const;

        /// Returns an optional timeout in seconds between successive CGI writes.
        // TODO: Not used until CGI is implemented.
        std::optional<Seconds>          cgi_write_timeout() const;

    private:
        std::vector<std::string> _ports;
        int                      _backlog;

        // Timeouts
        std::optional<Seconds>   _keepalive_timeout = Seconds(75);
        std::optional<Seconds>   _client_header_timeout = Seconds(60);
        std::optional<Seconds>   _client_body_timeout = Seconds(60);
        std::optional<Seconds>   _send_timeout = Seconds(60);
        Seconds                  _lingering_timeout = Seconds(5);
        std::optional<Seconds>   _cgi_read_timeout = Seconds(60);
        std::optional<Seconds>   _cgi_write_timeout = Seconds(60);
};
