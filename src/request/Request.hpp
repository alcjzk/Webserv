#pragma once

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "RequestLine.hpp"
#include "Response.hpp"
#include "Header.hpp"
#include "HttpUri.hpp"
#include "File.hpp"
#include "Method.hpp"
#include "Task.hpp"

class Server;

class Request
{
    public:
        using Connection = Response::Connection;
        using Headers = std::unordered_map<std::string, std::string>;

        class Builder
        {
            public:
                void header(Header&& header);
                void request_line(RequestLine&& request_line);

                const Headers& headers() const;

                Request build() &&;

                Headers                _headers;
                RequestLine            _request_line;
                Connection             _connection = Connection::KeepAlive;
                std::optional<HttpUri> _uri;
        };

        Task*          process(const Server& server, File&& file);
        const Method&  method() const;
        const Headers& headers() const;

        HttpUri     _uri;
        Connection  _connection;
        RequestLine _request_line;
        Headers     _headers;

    private:
        Request(
            HttpUri&& uri, Connection connection, RequestLine&& request_line, Headers&& headers
        );
};
