#include <cctype>
#include <string>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include "HTTPError.hpp"
#include "Status.hpp"
#include "ContentLength.hpp"

using std::string;

ContentLength::ContentLength(size_t value) noexcept : _value(value) {}

ContentLength::ContentLength(const std::string& value)
{
    if (value.empty())
        throw HTTPError(Status::BAD_REQUEST);

    auto not_digit = [](unsigned char c) { return !std::isdigit(c); };
    if (std::any_of(value.begin(), value.end(), not_digit))
        throw HTTPError(Status::BAD_REQUEST);

    try
    {
        _value = std::stoull(value);
    }
    catch (const std::out_of_range&)
    {
        throw HTTPError(Status::CONTENT_TOO_LARGE);
    }
}

ContentLength::operator size_t() noexcept
{
    return _value;
}

#ifdef TEST

#include "testutils.hpp"
#include <limits>

void ContentLengthTest::string_constructor_basic_test()
{
    BEGIN

    EXPECT(ContentLength(5) == 5);
    EXPECT(ContentLength("5") == 5);
    EXPECT(ContentLength("0") == 0);

    size_t max = std::numeric_limits<size_t>::max();
    EXPECT(ContentLength(std::to_string(max)) == max);

    END
}

void ContentLengthTest::string_constructor_invalid_test()
{
    BEGIN

    try
    {
        ContentLength value("");
        EXPECT(false);
    }
    catch (const HTTPError& error)
    {
        EXPECT(error.status().code() == Status::BAD_REQUEST);
    }
    try
    {
        ContentLength value("123abc");
        EXPECT(false);
    }
    catch (const HTTPError& error)
    {
        EXPECT(error.status().code() == Status::BAD_REQUEST);
    }
    try
    {
        ContentLength value("9859838909283020358028320385028358208593");
        EXPECT(false);
    }
    catch (const HTTPError& error)
    {
        EXPECT(error.status().code() == Status::CONTENT_TOO_LARGE);
    }

    END
}

#endif
