#include <cassert>
#include "HTTPError.hpp"
#include "Method.hpp"

using std::istream;
using std::string;

Method::Method(const std::string& str) : _type(type_from(str)) {}

Method::Type Method::type_from(const std::string& str)
{
    if (str == "GET")
        return Get;
    else if (str == "POST")
        return Post;
    else if (str == "DELETE")
        return Delete;
    else
        throw HTTPError(Status::BAD_REQUEST);
}

bool Method::operator==(const Method& other)
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

std::ostream& operator<<(std::ostream& os, const Method& method)
{
    return os << method.to_string();
}

Method::operator size_t() const
{
    return _type;
}
