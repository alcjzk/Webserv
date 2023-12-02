#include <cassert>
#include "defs.hpp"
#include "HTTPError.hpp"
#include "Method.hpp"

using std::istream;
using std::string;

Method::Method(const std::string& str) : _type(_type_from(str))
{

}

Method::Type Method::_type_from(const std::string& str)
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

Method::Method() : _type(Get)
{

}

Method::Method(Reader& reader) :
    _type(_type_from(reader.token(HTTP_FIELD_DELIMETER, STRING_MAX_LENGTH)))
{

}

bool Method::operator==(const Method& other)
{
    return _type == other._type;
}

const Method Method::deserialize(Reader& reader)
{
    const char* delimeters =  " \t";

    return Method(reader.trim(delimeters).token(delimeters));
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
