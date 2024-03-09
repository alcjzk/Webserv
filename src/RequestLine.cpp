#include "http.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"

using std::ostream;
using std::string;

RequestLine::RequestLine(const string& line)
{
    size_t head;
    size_t end;

    auto   it_end = line.begin() + std::min(line.length(), Method::MAX_LENGTH + 1);
    auto   it = std::find_first_of(line.begin(), line.end(), http::LWS.begin(), http::LWS.end());
    if (it == it_end)
        throw HTTPError(Status::NOT_IMPLEMENTED);
    _method = Method(string(line.begin(), it));

    end = std::distance(line.begin(), it);
    head = line.find_first_not_of(http::LWS, end);
    if (head == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    end = line.find_first_of(http::LWS, head);
    if (end == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    size_t length = end - head;
    if (length > URI_MAX_LENGTH)
        throw HTTPError(Status::URI_TOO_LONG);
    _request_target = line.substr(head, length);

    head = line.find_first_not_of(http::LWS, end);
    if (head == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    end = line.find_first_of(http::LWS, head);
    _http_version = HTTPVersion(line.substr(head, end - head));

    if (end != string::npos && line.find_first_not_of(http::LWS, end) != string::npos)
        throw HTTPError(Status::BAD_REQUEST);
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

void RequestLineTest::method_too_throws_not_implemented_test()
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
        request_line_str << '/' << std::string(RequestLine::URI_MAX_LENGTH, 'x');
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
    request_line_str << '/' << std::string(RequestLine::URI_MAX_LENGTH - 1, 'x');
    request_line_str << " HTTP/1.1";

    RequestLine request_line(request_line_str.str());

    END
}

#endif
