#include <algorithm>
#include "Log.hpp"
#include "Server.hpp"
#include "FileResponse.hpp"
#include "HTTPError.hpp"
#include "URI.hpp"
#include "Request.hpp"

using std::string;
using std::vector;

Response* Request::into_response(const Server& server) const
{
    const Header* host = header("Host");
    if (!host)
    {
        throw HTTPError(Status::BAD_REQUEST);
    }

    URI          request_uri(_request_line.request_target(), host->_value);
    const Route* route = server.route(request_uri.path());
    if (!route)
    {
        throw HTTPError(Status::BAD_REQUEST);
    }

    Path target = route->map(request_uri.path());

    if (target.type() == Path::Type::NOT_FOUND)
    {
        throw HTTPError(Status::NOT_FOUND);
    }
    if (target.type() != Path::Type::REGULAR)
    {
        throw HTTPError(Status::FORBIDDEN);
    }

    return new FileResponse(target);
}

const Method& Request::method() const
{
    return _request_line.method();
}

const HTTPVersion& Request::http_version() const
{
    return _request_line.http_version();
}

const Header* Request::header(const string& name) const
{
    auto header = std::find_if(_headers.cbegin(), _headers.cend(),
                               [name](const Header& header) { return header._name == name; });

    return header != _headers.cend() ? &(*header) : nullptr;
}
