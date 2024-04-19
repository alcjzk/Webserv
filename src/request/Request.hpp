#pragma once

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "ContentLength.hpp"
#include "RequestLine.hpp"
#include "Header.hpp"
#include "HttpUri.hpp"
#include "Method.hpp"
#include "Connection.hpp"
#include "Task.hpp"

class Server;

class Request
{
    public:
        using Headers = std::unordered_map<std::string, std::string>;
        using Body = std::vector<char>;

        class Builder
        {
            public:
                void body(Body&& body);
                void header(Header&& header);
                void request_line(RequestLine&& request_line);

                const Headers& headers() const;

                /// Returns the requests content-length based on received headers.
                ///
                /// @return value of the content-length header or 0 if not present.
                ///
                /// @throws HTTPError (see ContentLength).
                ContentLength content_length() const;

                Request build() &&;

                Headers                _headers;
                RequestLine            _request_line;
                bool                   _keep_alive = true;
                std::optional<HttpUri> _uri;
                Body                   _body;
        };

        Task*          process(Connection&& connection);
        const Method&  method() const;
        const Headers& headers() const;

        HttpUri     _uri;
        RequestLine _request_line;
        Headers     _headers;
        Body        _body;

    private:
        Request(HttpUri&& uri, RequestLine&& request_line, Headers&& headers, Body&& body);
};
