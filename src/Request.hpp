#pragma once

#include <vector>
#include <string>

class Request
{
    public:
        Request();

    private:
        std::string                 _request_line;
        std::vector<std::string>    _headers;

        typedef enum Expect
        {
            RequestLine,
            Headers,
            Body
        } Expect;
        Expect                  _expect;
        std::vector<char>    _buffer;
};
