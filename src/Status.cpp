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
        case MOVED_PERMANENTLY:
            return "302 Moved Permanently";
        case BAD_REQUEST:
            return "400 Bad Request";
        case FORBIDDEN:
            return "403 Forbidden";
        case NOT_FOUND:
            return "404 Not Found";
        case INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
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
