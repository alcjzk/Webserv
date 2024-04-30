#include <algorithm>
#include <utility>
#include <cctype>
#include "http.hpp"
#include "HTTPError.hpp"
#include "FieldName.hpp"

using std::string;

const FieldName FieldName::HOST = FieldName("host");
const FieldName FieldName::LOCATION = FieldName("location");
const FieldName FieldName::CONNECTION = FieldName("connection");
const FieldName FieldName::CONTENT_TYPE = FieldName("content-type");
const FieldName FieldName::CONTENT_LENGTH = FieldName("content-length");
const FieldName FieldName::TRANSFER_ENCODING = FieldName("transfer-encoding");
const FieldName FieldName::CONTENT_DISPOSITION = FieldName("content-disposition");

FieldName::FieldName(string&& name)
{
    if (!http::is_token(name))
        throw HTTPError(Status::BAD_REQUEST);
    auto to_lowercase = [](unsigned char c) { return std::tolower(c); };
    (void)std::transform(name.begin(), name.end(), name.begin(), to_lowercase);
    _value = std::move(name);
}

FieldName::FieldName(const string& name) : FieldName(string(name)) {}

FieldName::operator const string&() const noexcept
{
    return _value;
}

const string& FieldName::operator*() const noexcept
{
    return _value;
}

const string* FieldName::operator->() const noexcept
{
    return &_value;
}

bool operator==(const FieldName& lhs, const FieldName& rhs) noexcept
{
    return *lhs == *rhs;
}

bool operator!=(const FieldName& lhs, const FieldName& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const FieldName& name)
{
    return os << *name;
}

#ifdef TEST

#include "testutils.hpp"

void FieldNameTest::name_is_case_insensitive_test()
{
    BEGIN

    EXPECT(*FieldName("NAME") == *FieldName("name"));

    END
}

#endif
