#pragma once

class Status
{
    public:
        typedef enum Code
        {
            BAD_REQUEST = 400,
            NOT_FOUND = 404,
            INTERNAL_SERVER_ERROR = 500,
            HTTP_VERSION_NOT_SUPPORTED = 505
        } Code;

        Status(Code code);

        Code        code() const;
        const char* text() const;
        bool        is_error() const;

    private:
        Code _code;
};
