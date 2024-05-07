#include <cassert>
#include "HTTPError.hpp"
#include "Method.hpp"

using std::ostream;
using std::string;

Method::Method(Type type) noexcept : _type(type) {}

Method Method::from_string(const string& str)
{
    if (str == "GET")
        return Get;
    else if (str == "POST")
        return Post;
    else if (str == "DELETE")
        return Delete;
    else
        throw HTTPError(Status::NOT_IMPLEMENTED);
}

bool Method::operator==(const Method& other) noexcept
{
    return _type == other._type;
}

string Method::to_string() const
{
    switch (_type)
    {
        case Get:
            return "GET";
        case Post:
            return "POST";
        case Delete:
            return "DELETE";
        default:
            assert(false);
    }
}

ostream& operator<<(ostream& os, const Method& method)
{
    return os << method.to_string();
}

Method::operator size_t() const noexcept
{
    return _type;
}
