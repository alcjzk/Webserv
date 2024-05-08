#include <cassert>
#include "Error.hpp"

Error::Error(Kind kind) : std::runtime_error(kind_str(kind)), _kind(kind) {}

const char* Error::what() const noexcept
{
    return kind_str(_kind);
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
