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

        const Method&               method() const;
        const HTTPVersion&          http_version() const;
        const Header*               header(const std::string& name) const;
        const std::vector<char>&    body() const;

        Response*           into_response(const Server& server) const;

        std::vector<char>   _body;
        RequestLine         _request_line;
        std::vector<Header> _headers;
};
