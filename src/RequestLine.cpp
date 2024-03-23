#include "http.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"

using std::ostream;
using std::string;

// TODO: Cleanup
RequestLine::RequestLine(const string& line)
{
    size_t head;
    size_t end;

    head = line.find_first_not_of(http::LWS);
    if (head == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    end = line.find_first_of(http::LWS, head);
    if (end == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    _method = Method(line.substr(head, end - head));

    head = line.find_first_not_of(http::LWS, end);
    if (head == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    end = line.find_first_of(http::LWS, head);
    if (end == string::npos)
        throw HTTPError(Status::BAD_REQUEST);
    _request_target = line.substr(head, end - head);

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