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
        Error(const Error& other);
        virtual ~Error() throw();

        Error& operator=(const Error& other);
        bool operator==(const Error& other) const;
        bool operator==(Kind other) const;

        Kind kind() const;

        static const char*  kind_str(Kind kind);
    private:
        Kind    _kind;

};
