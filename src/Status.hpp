#pragma once

#include <string>
#include <iostream>

class Status
{
    public:
        typedef enum Code
        {
            OK = 200,
            PERM_REDIR = 301,
            BAD_REQUEST = 400,
            FORBIDDEN = 403,
            NOT_FOUND = 404,
            INTERNAL_SERVER_ERROR = 500,
            HTTP_VERSION_NOT_SUPPORTED = 505
        } Code;

        Status(Code code);

        Code        code() const;
        const char* text() const;
        bool        is_error() const;

        std::string as_status_line() const;

    private:
        Code _code;
};

std::ostream& operator<<(std::ostream& os, Status status);
