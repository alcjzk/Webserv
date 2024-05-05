#include <cassert>
#include "http.hpp"
#include "Status.hpp"

using std::string, std::ostream;

Status::Status(Code code) : _code(code) {}

Status::Code Status::code() const
{
    return _code;
}

const char* Status::text() const
{
    switch (_code)
    {
        case OK:
            return "200 OK";
        case CREATED:
            return "201 Created";
        case MOVED_PERMANENTLY:
            return "301 Moved Permanently";
        case BAD_REQUEST:
            return "400 Bad Request";
        case FORBIDDEN:
            return "403 Forbidden";
        case NOT_FOUND:
            return "404 Not Found";
        case REQUEST_TIMEOUT:
            return "408 Request Timeout";
        case CONFLICT:
            return "409 Conflict";
        case CONTENT_TOO_LARGE:
            return "413 Content Too Large";
        case URI_TOO_LONG:
            return "414 URI Too Long";
        case UNSUPPORTED_MEDIA_TYPE:
            return "415 Unsupported Media Type";
        case INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        case NOT_IMPLEMENTED:
            return "501 Not Implemented";
        case HTTP_VERSION_NOT_SUPPORTED:
            return "505 HTTP Version Not Supported";
        default:
            assert(false);
    }
}

bool Status::is_error() const
{
    if (_code >= 400)
        return true;
    return false;
}

string Status::as_status_line() const
{
    return http::VERSION.to_string() + ' ' + text();
}

ostream& operator<<(ostream& os, Status status)
{
    return os << status.text();
}
