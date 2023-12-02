#pragma once

#include <ostream>
#include "Method.hpp"
#include "URI.hpp"
#include "HTTPVersion.hpp"

// GET / HTTP/1.1

class RequestLine
{
    public:
        RequestLine();
        RequestLine(const std::string& line);

        const Method&       method() const;
        const URI&          uri() const;
        const HTTPVersion&  http_version() const;

    private:
        Method      _method;
        URI         _uri;
        HTTPVersion _http_version;
};

std::ostream& operator<<(std::ostream& os, const RequestLine& line);
