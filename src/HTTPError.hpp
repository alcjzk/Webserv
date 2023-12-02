#pragma once

#include <exception>
#include "Status.hpp"

class HTTPError : public std::exception
{
    public:
        HTTPError(Status status);
        HTTPError(const HTTPError& other);
        virtual ~HTTPError() throw();

        virtual const char* what() const throw();

        HTTPError& operator=(const HTTPError& other);

    private:
        Status  _status;
};
