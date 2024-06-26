#pragma once

#include <stdexcept>

class Error : public std::runtime_error
{
    public:
        typedef enum Kind
        {
            CLOSED
        } Kind;

        Error(Kind kind);

        virtual const char* what() const throw();

        bool operator==(const Error& other) const;
        bool operator==(Kind other) const;

        static const char* kind_str(Kind kind);

    private:
        Kind _kind;
};
