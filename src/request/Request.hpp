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
                void request_line(RequestLine&& request_line);

                const Headers& headers() const;

                /// Returns ContentLength if set.
                std::optional<ContentLength> content_length() const;

                void header(Header&& header);

                /// Returns a header value by key. Argument given must always be lowercase, or this
                /// function will never match.
                std::string*       header_by_key(const std::string& key);
                const std::string* header_by_key(const std::string& key) const;

                /// Parses the message headers.
                void parse_headers();

                Request build() &&;

                /// Converts a string to lowercase in-place.
                static void to_lower_in_place(std::string& value);

                Headers                      _headers;
                RequestLine                  _request_line;
                bool                         _keep_alive = true;
                std::optional<HttpUri>       _uri;
                std::optional<ContentLength> _content_length;
                Body                         _body;
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
