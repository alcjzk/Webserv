#pragma once

#include <vector>
#include <string>
#include <chrono>

class Config
{
    public:
        using Seconds = std::chrono::seconds;

        Config();

        const std::vector<std::string>& ports() const;
        int                             backlog() const;

        /// Returns a timeout in seconds for keeping an inactive connection alive.
        Seconds                         keepalive_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving request headers.
        Seconds                         client_header_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving a request body.
        // TODO: Not used until receive client body is implemented.
        Seconds                         client_body_timeout() const;

        /// Returns a timeout in seconds between successive writes when sending a response back to
        /// the client.
        Seconds                         send_timeout() const;

        /// Returns the maximum time to linger a connection.
        // TODO: Not used until lingering close is implemented.
        Seconds                         lingering_timeout() const;

        /// Returns a timeout in seconds between successive CGI reads.
        // TODO: Not used until CGI is implemented.
        Seconds                         cgi_read_timeout() const;

        /// Returns a timeout in seconds between successive CGI writes.
        // TODO: Not used until CGI is implemented.
        Seconds                         cgi_write_timeout() const;

    private:
        std::vector<std::string> _ports;
        int                      _backlog;

        // Timeouts
        Seconds                  _keepalive_timeout = Seconds(75);
        Seconds                  _client_header_timeout = Seconds(60);
        Seconds                  _client_body_timeout = Seconds(60);
        Seconds                  _send_timeout = Seconds(60);
        Seconds                  _lingering_timeout = Seconds(5);
        Seconds                  _cgi_read_timeout = Seconds(60);
        Seconds                  _cgi_write_timeout = Seconds(60);
};
