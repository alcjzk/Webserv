#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <cstddef>
#include "Status.hpp"
#include "HostAttributes.hpp"

class Config
{
    public:
        using Seconds = std::chrono::seconds;

        Config() = delete;
        Config(const Config& other) = default;
        Config& operator=(const Config& other) = default;
        Config(Config&& other) = default;
        Config& operator=(Config&& other) = default;
        Config(
            std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root,
            std::pair<std::string, TiniNode*> first_pair
        );

        const std::string&                 port() const;
        const std::string&                 host() const;
        int                                backlog() const;
        const std::vector<HostAttributes>& attrs() const;
        const HostAttributes&              first_attr() const;
        size_t                             header_buffsize() const;
        const std::string&                 error_str() const;
        std::optional<Path>                error_page(Status status) const;

        /// Returns the maximum request body size.
        size_t body_size() const;

        /// Returns a timeout in seconds for keeping an inactive connection alive.
        Seconds keepalive_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving request headers.
        Seconds client_header_timeout() const;

        /// Returns a timeout in seconds between successive reads when receiving a request body.
        // TODO: Not used until receive client body is implemented.
        Seconds client_body_timeout() const;

        /// Returns a timeout in seconds between successive writes when sending a response back to
        /// the client.
        Seconds send_timeout() const;

        /// Returns the maximum time to linger a connection.
        // TODO: Not used until lingering close is implemented.
        Seconds lingering_timeout() const;

        /// Returns a timeout in seconds between successive CGI reads.
        // TODO: Not used until CGI is implemented.
        Seconds cgi_read_timeout() const;

        /// Returns a timeout in seconds between successive CGI writes.
        // TODO: Not used until CGI is implemented.
        Seconds cgi_write_timeout() const;

        /// Returns a timeout in seconds between successive internal io reads.
        Seconds io_read_timeout() const;

        /// Returns a timeout in seconds between successive internal io writes.
        Seconds io_write_timeout() const;

    private:
        // Timeouts
        Seconds _keepalive_timeout = Seconds(75);
        Seconds _client_header_timeout = Seconds(60);
        Seconds _client_body_timeout = Seconds(60);
        Seconds _send_timeout = Seconds(60);
        Seconds _lingering_timeout = Seconds(5);
        Seconds _cgi_read_timeout = Seconds(5);
        Seconds _cgi_write_timeout = Seconds(5);
        Seconds _io_read_timeout = Seconds(60);
        Seconds _io_write_timeout = Seconds(60);

        std::string                 _port = "8000";
        std::string                 _host = "127.0.0.1";
        std::vector<HostAttributes> _attrs;
        HostAttributes              _first_attr;
        int                         _backlog = 128;
        size_t                      _body_size = 4096;
        size_t                      _header_buffer_size = 1024;
        std::map<int, Path>         _error_pages;
        std::string                 _error_template = R"(<!DOCTYPE html>
                                                        <html lang="en">
                                                        <head>
                                                            <meta charset="UTF-8">
                                                            <meta name="viewport" content="width=device-width, initial-scale=1.0">
                                                            <title>Error</title>
                                                            <style>
                                                                body {
                                                                    font-family: Arial, sans-serif;
                                                                    text-align: center;
                                                                    padding: 20px;
                                                                }
                                                                h1 {
                                                                    color: #ff0000;
                                                                }
                                                                p {
                                                                    font-size: 18px;
                                                                }
                                                            </style>
                                                        </head>
                                                        <body>
                                                            <h1>Error</h1>
                                                            <p>An error has occurred. Please try again later.</p>
                                                            <p>Error Code: {{status}}</p>
                                                        </body>
                                                        </html>)";
};
