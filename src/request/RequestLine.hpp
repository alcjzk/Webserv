#pragma once

#include <ostream>
#include "Method.hpp"
#include "HTTPVersion.hpp"

class RequestLine
{
    public:
        static const size_t     MAX_LENGTH = 8000;

        static constexpr size_t uri_max_length();

        RequestLine() = default;
        RequestLine(const std::string& line);

        const Method&      method() const;
        const std::string& request_target() const;
        const HTTPVersion& http_version() const;

    private:
        Method      _method;
        HTTPVersion _http_version;
        std::string _request_target;
};

std::ostream& operator<<(std::ostream& os, const RequestLine& line);

#ifdef TEST

class RequestLineTest : public RequestLine
{
    public:
        // A server that receives a method longer than any that it implements SHOULD respond with a
        // 501 (Not Implemented) status code.
        static void method_too_throws_not_implemented_test();

        //  A server that receives a request-target longer than any URI it wishes to parse MUST
        // respond with a 414 (URI Too Long) status code
        static void uri_too_long_test();
        static void uri_max_length_test();
};

#endif
