#include "HTTPError.hpp"
#include "defs.hpp"
#include "Header.hpp"

using std::string;

bool is_ctl(char c)
{
    if (c == DEL || c <= 31)
        return true;
    return false;
}

/// Returns true if the character is a separator, as defined by the RFC
bool is_separator(char c)
{
    const std::string separators("()<>@,;:\\\"/[]?={} \t");

    for (char separator : separators)
    {
        if (c == separator)
            return true;
    }
    return false;
}

/// Returns true if the input is a valid token, as defined by the RFC
bool is_token(const std::string& text)
{
    for (char c : text)
    {
        if (is_ctl(c) || is_separator(c))
            return false;
    }
    return true;
}


Header::Header(string name, string value) :
    _name(name),
    _value(value) {}

Header::Header(const string& text)
{
    size_t start_pos = text.find_first_not_of(HTTP_LWS);
    size_t end_pos = text.find_first_of(':', start_pos);

    if (end_pos == string::npos)
    {
        throw HTTPError(Status::BAD_REQUEST);
    }
    _name = text.substr(0, end_pos);
    if (!is_token(_name))
    {
        throw HTTPError(Status::BAD_REQUEST);
    }
    start_pos = text.find_first_not_of(HTTP_LWS, end_pos + 1);
    end_pos = text.find_last_not_of(HTTP_LWS, start_pos);
    if (end_pos != string::npos)
    {
        _value = text.substr(start_pos, end_pos - start_pos);
    }
}

void Header::append(const string& value)
{
    size_t start_pos = value.find_first_not_of(HTTP_LWS);
    size_t end_pos = value.find_last_not_of(HTTP_LWS);

    if (end_pos == string::npos)
    {
        return ; // Ignore appending empty values
    }
    _value.append(1, SP);
    _value.append(value.substr(start_pos, end_pos - start_pos));
}
