#include "HTTPError.hpp"

HTTPError::HTTPError(Status status) : _status(status)
{
    // TODO: Disallow constructing from non-error status
}

HTTPError::HTTPError(const HTTPError& other) : _status(other._status)
{

}

HTTPError::~HTTPError() throw()
{

}

const char* HTTPError::what() const throw()
{
    return _status.text();
}

HTTPError& HTTPError::operator=(const HTTPError& other)
{
    _status = other._status;
    return *this;
}
