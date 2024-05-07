#pragma once

#include <vector>
#include <string>
#include <optional>
#include "ContentLength.hpp"
#include "RequestLine.hpp"
#include "HttpUri.hpp"
#include "Method.hpp"
#include "Connection.hpp"
#include "Task.hpp"
#include "FieldMap.hpp"

class Server;

class Request
{
    public:
        using Body = std::vector<char>;

        class Builder
        {
            public:
                void body(Body&& body);
                void request_line(RequestLine&& request_line);

                FieldMap&       headers();
                const FieldMap& headers() const;

                /// Returns ContentLength if set.
                ///
                /// Must not be called before `parse_headers`.
                std::optional<ContentLength> content_length() const;

                /// Returns true if the request uses chunked transfer-encoding.
                ///
                /// Must not be called before `parse_headers`.
                bool is_chunked() const;

                void header(const std::string& header);

                /// Parses the message headers.
                void parse_headers();

                Request build() &&;

                FieldMap                     _headers;
                RequestLine                  _request_line;
                bool                         _keep_alive = true;
                bool                         _is_chunked = false;
                std::optional<HttpUri>       _uri;
                std::optional<ContentLength> _content_length;
                Body                         _body;
        };

        Request(Builder&& builder);

        Task*              process(Connection&& connection);
        const HttpUri&     uri() const;
        const RequestLine& request_line() const;
        const Method&      method() const;
        const FieldMap&    headers() const;
        const Body&        body() const&;
        Body&&             body() &&;

    private:
        HttpUri     _uri;
        RequestLine _request_line;
        FieldMap    _headers;
        Body        _body;
        bool        _keep_alive;
};
