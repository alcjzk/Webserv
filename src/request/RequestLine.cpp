#include <algorithm>
#include "http.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"

using std::ostream;
using std::string;

constexpr size_t RequestLine::uri_max_length()
{
    return MAX_LENGTH - Method::MAX_LENGTH - HTTPVersion::MAX_LENGTH - 2;
}

RequestLine::RequestLine(const string& line)
{
    auto first = line.begin();
    auto end = first + std::min(line.length(), Method::MAX_LENGTH + 1);
    auto last = std::find(first, end, http::SP);
    if (last == end)
        throw HTTPError(Status::NOT_IMPLEMENTED);
    _method = Method::from_string(string(first, last));

    first = std::next(last);
    end = line.end();
    last = std::find(first, end, http::SP);
    if (last == end)
        throw HTTPError(Status::BAD_REQUEST);
    if (std::distance(first, last) > (ssize_t)uri_max_length())
        throw HTTPError(Status::URI_TOO_LONG);
    _request_target = string(first, last);

    _http_version = HTTPVersion(string(std::next(last), end));
}

const Method& RequestLine::method() const
{
    return _method;
}

const string& RequestLine::request_target() const
{
    return _request_target;
}

const HTTPVersion& RequestLine::http_version() const
{
    return _http_version;
}

ostream& operator<<(ostream& os, const RequestLine& line)
{
    return os << line.method() << ' ' << line.request_target() << ' ' << line.http_version();
}

#ifdef TEST

#include <sstream>
#include <string>
#include "testutils.hpp"

void RequestLineTest::method_too_long_throws_not_implemented_test()
{
    BEGIN

    try
    {
        RequestLine("DELETER / HTTP/1.1\r\n");
    }
    catch (const HTTPError& error)
    {
        EXPECT(error.status().code() == Status::NOT_IMPLEMENTED);
        return;
    }
    EXPECT(false);

    END
}

void RequestLineTest::uri_too_long_test()
{
    BEGIN

    try
    {
        std::stringstream request_line_str;

        request_line_str << "GET ";
        request_line_str << '/' << std::string(RequestLine::uri_max_length(), 'x');
        request_line_str << " HTTP/1.1";

        RequestLine request_line(request_line_str.str());
    }
    catch (const HTTPError& error)
    {
        EXPECT(error.status().code() == Status::URI_TOO_LONG);
        return;
    }
    EXPECT(false);

    END
}

void RequestLineTest::uri_max_length_test()
{
    BEGIN

    std::stringstream request_line_str;

    request_line_str << "GET ";
    request_line_str << '/' << std::string(RequestLine::uri_max_length() - 1, 'x');
    request_line_str << " HTTP/1.1";

    RequestLine request_line(request_line_str.str());

    END
}

#endif
