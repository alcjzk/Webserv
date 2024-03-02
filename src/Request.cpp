#include <algorithm>
#include "Log.hpp"
#include "Server.hpp"
#include "FileResponse.hpp"
#include "DirectoryResponse.hpp"
#include "RedirectionResponse.hpp"
#include "HTTPError.hpp"
#include "URI.hpp"
#include "Request.hpp"
#include "TiniUtils.hpp"

using std::string;

Response* Request::into_response(const Server& server) const
{
    const Header* host = header("Host");
    if (!host)
        throw HTTPError(Status::BAD_REQUEST);
    std::string  hostname = split(host->_value, ":")[0];
    URI          request_uri(_request_line.request_target(), host->_value);

    const Route* route = server.route(request_uri.path(), hostname);
    if (!route)
        throw HTTPError(Status::BAD_REQUEST);

    // Redirect does not depend on request_uri mapping to a valid target directory on the host
    if (route->_type == Route::REDIRECTION)
        return new RedirectionResponse(route->_redir.value());

    Path target = route->map(request_uri.path());

    if (!route->method_get())
        throw HTTPError(Status::FORBIDDEN);

    if (target.type() == Path::Type::NOT_FOUND)
        throw HTTPError(Status::NOT_FOUND);

    // Can add CGI suffix parsing here later
    if (target.type() == Path::Type::REGULAR)
        return new FileResponse(target);

    if (target.type() != Path::Type::DIRECTORY)
        throw HTTPError(Status::FORBIDDEN);

    // At this point we know it's a directory, can remove a layer of nesting
    try
    {
        if (route->_default_file.has_value())
            return new FileResponse(target + Path(route->_default_file.value()));
    }
    catch (const std::exception& e)
    {
        INFO("Request::into_response: " << e.what());
        if (server.map_attributes(hostname).dirlist())
            return new DirectoryResponse(target, request_uri.path());
        throw HTTPError(Status::NOT_FOUND);
    }
    if (!server.map_attributes(hostname).dirlist())
        throw HTTPError(Status::FORBIDDEN);
    return new DirectoryResponse(target, request_uri.path());
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
