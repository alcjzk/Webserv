#include <stdexcept>
#include "HTTPError.hpp"

HTTPError::HTTPError(Status status) : _status(status)
{
    if (!status.is_error())
        throw std::logic_error("HTTPError: attempt to construct from non-error status");
}

const char* HTTPError::what() const throw()
{
    return _status.text();
}

Status HTTPError::status() const
{
    return _status;
}
