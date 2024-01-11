#pragma once

#include <ostream>
#include "Method.hpp"
#include "URI.hpp"
#include "HTTPVersion.hpp"

// GET / HTTP/1.1

class RequestLine
{
    public:
        RequestLine() = default;
        RequestLine(const std::string& line);

        const Method&      method() const;
        const std::string& request_target() const;
        const HTTPVersion& http_version() const;

        // request_target = RFC 9112 (HTTP/1.1)
        // - origin-form (todo) |
        // - absolute-form (WONT IMPLEMENT used only for proxies) |
        // - authority-form (WONT IMPLEMENT used only for CONNECT) |
        // - asterisk-form (WONT IMPLEMENT used only for OPTIONS)

        // origin-form (RFC 9112 (HTTP/1.1)) = absolute-path [ "?" query ]

        // absolute-path (RFC 9110 HTTP Semantics) = 1*( "/" segment )

        // segment (RFC 3986 URI) = *pchar
        // query  (RFC 3986 URI) = *( pchar / "/" / "?" )
        // pchar (RFC 3986 URI) = unreserved / pct-encoded / sub-delims / ":" / "@"

        // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
        // pct-encoded = "%" HEXDIG HEXDIG
        // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
        //             / "*" / "+" / "," / ";" / "="

        //

    private:
        Method      _method;
        HTTPVersion _http_version;
        std::string _request_target;
};

std::ostream& operator<<(std::ostream& os, const RequestLine& line);
