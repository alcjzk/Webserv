#pragma once

#include <exception>
#include "Status.hpp"

class HTTPError : public std::exception
{
    public:
        /// Construct from a HTTP status.
        ///
        /// @throws std::logic_error - status is not an error.
        HTTPError(Status status);

        virtual const char* what() const noexcept;

        Status status() const;

    private:
        Status _status;
};
