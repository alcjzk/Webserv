#include "Request.hpp"

Response* Request::into_response() const
{
    // TODO: Implement responses based on the request
    return new TextResponse("Got request");
}

const Method& Request::method() const
{
    return _request_line.method();
}

const URI& Request::uri() const
{
    return _request_line.uri();
}

const HTTPVersion& Request::http_version() const
{
    return _request_line.http_version();
}
