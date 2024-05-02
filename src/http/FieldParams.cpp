#include <regex>
#include <cassert>
#include "FieldParams.hpp"
#include "HTTPError.hpp"
#include "Status.hpp"
#include "http.hpp"

using std::string;
using std::string_view;
using Value = FieldParams::Value;
using Key = FieldParams::Key;

static inline void to_lowercase_in_place(std::string& value)
{
    constexpr auto to_lower = [](unsigned char c) { return std::tolower(c); };
    (void)std::transform(value.begin(), value.end(), value.begin(), to_lower);
}

FieldParams::FieldParams(const string_view& value)
{
    using Iterator = std::regex_iterator<string_view::const_iterator>;
    static const std::regex regex(R"((?:[ \t]*);(?:[ \t]*)([^ \t;=]+)=([^ \t;=]+))");

    Iterator match(value.begin(), value.end(), regex);
    Iterator end;

    for (; match != end; match++)
    {
        assert(match->size() == 3);

        Key key((*match)[1]);
        to_lowercase_in_place(key);

        Value value((*match)[2]);
        if (!http::is_token(value))
            value = parse_quoted(value);

        _inner.emplace<Entry>({std::move(key), std::move(value)});
    }
}

const Value* FieldParams::get(const Key& key) const
{
    if (auto it = _inner.find(key); it != _inner.end())
    {
        return &(it->second);
    }
    return nullptr;
}

Value FieldParams::parse_quoted(const Value& value)
{
    if (value.length() < 2)
        throw HTTPError(Status::BAD_REQUEST);
    if (value.front() != http::DQUOTE || value.back() != http::DQUOTE)
        throw HTTPError(Status::BAD_REQUEST);

    string result;
    result.reserve(value.length() - 2);
    auto head = value.cbegin() + 1;
    while (head != value.end() - 1)
    {
        if (!http::is_qdtext(*head))
        {
            if (*head != '\\')
                throw HTTPError(Status::BAD_REQUEST);
            head = std::next(head);
            if (!http::is_whitespace(*head) && !http::is_field_vchar(*head))
                throw HTTPError(Status::BAD_REQUEST);
        }
        result.push_back(*head);
        head = std::next(head);
    }
    return result;
}
