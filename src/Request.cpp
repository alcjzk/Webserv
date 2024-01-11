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
    auto routes = server.routes();

    INFO(_request_line.request_target());

    auto host = header("Host");
    if (host == _headers.end())
    {
        throw HTTPError(Status::BAD_REQUEST);
    }

    URI request_uri(_request_line.request_target(), host->_value);

    INFO(request_uri.path());

    auto route =
        std::find_if(routes.cbegin(), routes.cend(),
                     [request_uri](const Route& route) { return route.match(request_uri.path()); });

    if (route == routes.cend())
    {
        throw HTTPError(Status::BAD_REQUEST);
    }

    auto target = route->map(request_uri.path());
    INFO("Target resource: " << target);

    auto status = std::filesystem::status(target);
    if (status.type() == std::filesystem::file_type::not_found)
    {
        throw HTTPError(Status::NOT_FOUND);
    }
    if (status.type() != std::filesystem::file_type::regular)
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

vector<Header>::const_iterator Request::header(const string& name) const
{
    return std::find_if(_headers.begin(), _headers.end(),
                        [name](const Header& header) { return header._name == name; });
}
