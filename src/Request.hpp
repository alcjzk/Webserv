#pragma once

#include <vector>
#include <string>
#include "RequestLine.hpp"
#include "Response.hpp"
#include "Header.hpp"

class Server;

class Request
{
    public:
        Request() = default;

        const Method&                       method() const;
        const HTTPVersion&                  http_version() const;
        std::vector<Header>::const_iterator header(const std::string& name) const;

        Response*                           into_response(const Server& server) const;

        RequestLine                         _request_line;
        std::vector<Header>                 _headers;
};
