#include <utility>
#include "http.hpp"
#include "HTTPError.hpp"
#include "FieldValue.hpp"

using std::ostream;
using std::pair;
using std::string;

const FieldValue FieldValue::CLOSE = FieldValue("close");

FieldValue::FieldValue(string&& value)
{
    auto is_valid = [](unsigned char c)
    { return http::is_field_vchar(c) || http::is_whitespace(c); };

    if (value.empty())
        return;

    if (http::is_whitespace(value.front()) || http::is_whitespace(value.back()))
        throw HTTPError(Status::BAD_REQUEST);

    if (!std::all_of(value.begin(), value.end(), is_valid))
        throw HTTPError(Status::BAD_REQUEST);

    _value = std::move(value);
}

FieldValue::operator const string&() const noexcept
{
    return _value;
}

const string& FieldValue::operator*() const noexcept
{
    return _value;
}

const string* FieldValue::operator->() const noexcept
{
    return &_value;
}

ostream& operator<<(ostream& os, const FieldValue& value)
{
    return os << value._value;
}

pair<FieldValue, FieldParams> FieldValue::split() const
{
    auto it = std::find((*this)->begin(), (*this)->end(), ';');
    if (it == (*this)->end())
        return {*this, FieldParams()};

    auto rev_it = std::make_reverse_iterator(it);
    while (rev_it != (*this)->rend() && http::is_whitespace(*rev_it))
    {
        rev_it--;
    }
    return {string((*this)->begin(), rev_it.base()), FieldParams(string(it, (*this)->end()))};
}

FieldValue::FieldValue(const string& value) : FieldValue(string(value)) {}
