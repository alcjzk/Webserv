#pragma once

#include <exception>
#include "Status.hpp"

class HTTPError : public std::exception
{
    public:
        HTTPError(Status status);

        virtual const char* what() const throw();

    private:
        Status _status;
};
