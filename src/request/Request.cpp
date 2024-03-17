#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "Log.hpp"
#include "Server.hpp"
#include "FileResponse.hpp"
#include "DirectoryResponse.hpp"
#include "RedirectionResponse.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "HttpUri.hpp"

using std::string;

Response* Request::into_response(const Server& server) const
{
    try
    {
        Response::Connection connection = Response::Connection::KeepAlive;
        const Header*        host = header("Host");

        if (!host)
            throw HTTPError(Status::BAD_REQUEST);

        HttpUri      uri(_request_line.request_target(), host->_value);
        const Route* route = server.route(uri.path(), uri.host());
        if (!route)
            throw HTTPError(Status::BAD_REQUEST);

        if (const Header* connection_header = header("Connection"))
        {
            string value(connection_header->_value);

            (void)std::transform(value.begin(), value.end(), value.begin(), toupper);
            if (connection_header->_value == "close")
                connection = Response::Connection::Close;
        }

        if (route->_type == Route::REDIRECTION)
            return new RedirectionResponse(route->_redir.value(), connection);

        Path target = route->map(uri.path());

        if (!route->method_get())
            throw HTTPError(Status::FORBIDDEN);

        if (target.type() == Path::Type::NOT_FOUND)
            throw HTTPError(Status::NOT_FOUND);

        if (target.type() == Path::Type::REGULAR)
            return new FileResponse(target, connection);

        if (target.type() != Path::Type::DIRECTORY)
            throw HTTPError(Status::FORBIDDEN);

        try
        {
            if (route->_default_file.has_value())
                return new FileResponse(target + Path(route->_default_file.value()), connection);
        }
        catch (const std::exception& e)
        {
            INFO("Request::into_response: " << e.what());
            if (server.map_attributes(uri.host()).dirlist())
                return new DirectoryResponse(target, uri.path(), connection);
            throw HTTPError(Status::NOT_FOUND);
        }
        if (!server.map_attributes(uri.host()).dirlist())
            throw HTTPError(Status::FORBIDDEN);
        return new DirectoryResponse(target, uri.path(), connection);
    }
    catch (const std::runtime_error& error)
    {
        WARN("Request::into_response: " << error.what());
        throw HTTPError(Status::BAD_REQUEST);
    }
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
