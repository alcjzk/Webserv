#include "http.hpp"
#include <algorithm>
#include <cctype>
#include <string_view>

using namespace http;

using std::pair;
using std::string;
using std::string_view;

bool http::is_token(const string& text)
{
    if (text.empty())
        return false;
    return std::all_of(text.begin(), text.end(), is_tchar);
}

bool http::is_tchar(unsigned char c)
{
    constexpr const string_view charset(R"(!#$%&'*+-.^_`|~)");

    if (std::isalnum(c) || charset.find(c) != string_view::npos)
        return true;
    return false;
}

bool http::is_whitespace(unsigned char c)
{
    return (c == SP) || (c == HTAB);
}

bool http::is_field_vchar(unsigned char c)
{
    if (std::isgraph(c) || http::is_obs_text(c))
        return true;
    return false;
}

bool http::is_qdtext(unsigned char c)
{
    if (http::is_whitespace(c) || http::is_obs_text(c))
        return true;
    if (c == 0x21)
        return true;
    if (c >= 0x23 && c <= 0x5B)
        return true;
    if (c >= 0x5D && c <= 0x7E)
        return true;
    return false;
}

bool http::is_obs_text(unsigned char c)
{
    return c >= 0x80;
}

pair<FieldName, FieldValue> http::parse_field(const string& field)
{
    auto   delimeter = std::find(field.begin(), field.end(), ':');
    string name(field.begin(), delimeter);
    string value;

    if (delimeter != field.end())
    {
        auto begin = std::find_if_not(std::next(delimeter), field.end(), http::is_whitespace);
        if (begin != field.end())
        {
            auto end = std::find_if_not(
                field.rbegin(), std::make_reverse_iterator(begin), http::is_whitespace
            );
            value = string(begin, end.base());
        }
    }
    return {FieldName(std::move(name)), value};
}
