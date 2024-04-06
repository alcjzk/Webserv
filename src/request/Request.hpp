#pragma once

#include <vector>
#include <string>
#include <optional>
#include "RequestLine.hpp"
#include "Response.hpp"
#include "Header.hpp"
#include "HttpUri.hpp"
#include "Task.hpp"

class Server;

class Request
{
    public:
        using Connection = Response::Connection;

        class Builder
        {
            public:
                void header(Header&& header);
                void request_line(RequestLine&& request_line);

                Request build() &&;

                std::vector<Header>    _headers;
                RequestLine            _request_line;
                Connection             _connection = Connection::KeepAlive;
                std::optional<HttpUri> _uri;
        };

        Task*         process(const Server& server, File&& file);
        const Method& method() const;
        const Header* header(const std::string& name) const;

        HttpUri             _uri;
        Connection          _connection;
        RequestLine         _request_line;
        std::vector<Header> _headers;

    private:
        Request(
            HttpUri&& uri, Connection connection, RequestLine&& request_line,
            std::vector<Header>&& headers
        );
};
