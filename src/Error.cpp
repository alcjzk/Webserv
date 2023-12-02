#include <cassert>
#include "Error.hpp"

Error::Error(Kind kind) : std::runtime_error(kind_str(kind)), _kind(kind)
{

}

Error::Error(const Error& other) : std::runtime_error(other), _kind(other._kind)
{

}

Error::~Error() throw()
{

}

Error& Error::operator=(const Error& other)
{
    (void)std::runtime_error::operator=(other);
    _kind = other._kind;
    return *this;
}

bool Error::operator==(const Error& other) const
{
    return _kind == other._kind;
}

bool Error::operator==(Kind kind) const
{
    return _kind == kind;
}

Error::Kind Error::kind() const
{
    return _kind;
}

const char* Error::kind_str(Kind kind)
{
    switch (kind)
    {
        case CLOSED:
            return "Connection was closed by the client";
        default:
            assert(false);
    }
}
