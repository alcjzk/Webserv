#include "http.hpp"
#include "HTTPError.hpp"
#include "Header.hpp"

using std::string;

Header::Header(string name, string value) :
    _name(name),
    _value(value) {}

Header::Header(const string& text)
{
    size_t start_pos = text.find_first_not_of(http::LWS);
    size_t end_pos = text.find_first_of(':', start_pos);

    if (end_pos == string::npos)
    {
        throw HTTPError(Status::BAD_REQUEST);
    }
    _name = text.substr(0, end_pos);
    if (!http::is_token(_name))
    {
        throw HTTPError(Status::BAD_REQUEST);
    }
    start_pos = text.find_first_not_of(http::LWS, end_pos + 1);
    if (start_pos != string::npos)
    {
        end_pos = text.find_last_not_of(http::LWS);
        _value = text.substr(start_pos, end_pos - start_pos + 1);
    }
}

void Header::append(const string& value)
{
    size_t start_pos = value.find_first_not_of(http::LWS);
    size_t end_pos = value.find_last_not_of(http::LWS);

    if (end_pos == string::npos)
    {
        return ; // Ignore appending empty values
    }
    _value.append(1, http::SP);
    _value.append(value.substr(start_pos, end_pos - start_pos));
}

std::ostream& operator<<(std::ostream& os, const Header& header)
{
    return os << header._name << ": " << header._value;
}
