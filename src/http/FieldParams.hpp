#pragma once

#include <string>
#include <utility>
#include <string_view>
#include <unordered_map>

class FieldParams
{
    public:
        using Key = std::string;
        using Value = std::string;
        using Container = std::unordered_map<Key, Value>;
        using Entry = std::pair<Key, Value>;

        FieldParams() noexcept = default;
        FieldParams(const std::string_view& value);

        const Value* get(const Key& key) const;

    private:
        Container _inner;

        /// Parses and validates `value` as a quoted-string (RFC 9110).
        static Value parse_quoted(const Value& value);
};
