#pragma once

#include <string>
#include <iostream>

class Status
{
    public:
        typedef enum Code
        {
            OK = 200,
            CREATED = 201,
            MOVED_PERMANENTLY = 301,
            BAD_REQUEST = 400,
            FORBIDDEN = 403,
            NOT_FOUND = 404,
            METHOD_NOT_ALLOWED = 405,
            REQUEST_TIMEOUT = 408,
            CONFLICT = 409,
            CONTENT_TOO_LARGE = 413,
            UNSUPPORTED_MEDIA_TYPE = 415,
            URI_TOO_LONG = 414,
            INTERNAL_SERVER_ERROR = 500,
            NOT_IMPLEMENTED = 501,
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
