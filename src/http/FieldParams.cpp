#include <regex>
#include <cassert>
#include "FieldParams.hpp"

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

        Key   key((*match)[1]);
        Value value((*match)[2]);
        to_lowercase_in_place(key);

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
