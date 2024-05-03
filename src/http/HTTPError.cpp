#include "HTTPError.hpp"

HTTPError::HTTPError(Status status) : _status(status)
{
    // TODO: Disallow constructing from non-error status
}

const char* HTTPError::what() const throw()
{
    return _status.text();
}

Status HTTPError::status() const
{
    return _status;
}
