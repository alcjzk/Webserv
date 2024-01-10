#include <algorithm>
#include "Request.hpp"

using std::string;
using std::vector;

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

vector<Header>::const_iterator Request::header(const string& name) const
{
    return std::find_if(_headers.begin(), _headers.end(),
                        [name](const Header& header) { return header._name == name; });
}
