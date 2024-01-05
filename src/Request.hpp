#pragma once

#include <vector>
#include <string>
#include "RequestLine.hpp"
#include "Response.hpp"

class Request
{
    public:
        Request() = default;

        const Method&       method() const;
        const URI&          uri() const;
        const HTTPVersion&  http_version() const;

        Response*           into_response() const;

        RequestLine         _request_line;

    private:
        std::vector<std::string>    _headers;
};
