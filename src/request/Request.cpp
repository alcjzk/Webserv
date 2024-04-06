#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>
#include "Log.hpp"
#include "RequestLine.hpp"
#include "Server.hpp"
#include "FileResponse.hpp"
#include "DirectoryResponse.hpp"
#include "RedirectionResponse.hpp"
#include "SendResponseTask.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "HttpUri.hpp"

using std::string;

void Request::Builder::header(Header&& header)
{
    // TODO: Protect against duplicates
    try
    {
        if (header._name == "Host")
        {
            _uri = HttpUri(_request_line.request_target(), header._value);
        }
        else if (header._name == "Connection")
        {
            (void)std::transform(
                header._value.begin(), header._value.end(), header._value.begin(), toupper
            );
            if (header._value == "close")
            {
                _connection = Connection::Close;
            }
        }
        _headers.push_back(std::move(header));
    }
    catch (const std::runtime_error& error)
    {
        WARN("RequestBuilder::header: `" << header._value << "`: " << error.what());
        throw HTTPError(Status::BAD_REQUEST);
    }
}

void Request::Builder::request_line(RequestLine&& request_line)
{
    if (!request_line.http_version().is_compatible_with(Server::http_version()))
        throw HTTPError(Status::HTTP_VERSION_NOT_SUPPORTED);
    _request_line = std::move(request_line);
}

Request Request::Builder::build() &&
{
    if (!_uri)
        throw HTTPError(Status::BAD_REQUEST);
    return Request(std::move(*_uri), _connection, std::move(_request_line), std::move(_headers));
}

Request::Request(
    HttpUri&& uri, Connection connection, RequestLine&& request_line, std::vector<Header>&& headers
)
    : _uri(std::move(uri)), _connection(connection), _request_line(std::move(request_line)),
      _headers(std::move(headers))
{
}

Task* Request::process(const Server& server, File&& file)
{
    Response* response;

    const Route* route = server.route(_uri.path(), _uri.host());

    if (route->_type == Route::REDIRECTION)
    {
        response = new RedirectionResponse(route->_redir.value(), _connection);
        return new SendResponseTask(server, std::move(file), response);
    }

    Path target = route->map(_uri.path());
    if (!route->method_get())
        throw HTTPError(Status::FORBIDDEN);

    if (target.type() == Path::Type::NOT_FOUND)
        throw HTTPError(Status::NOT_FOUND);

    if (target.type() == Path::Type::REGULAR)
    {
        response = new FileResponse(target, _connection);
        return new SendResponseTask(server, std::move(file), response);
    }

    if (target.type() != Path::Type::DIRECTORY)
        throw HTTPError(Status::FORBIDDEN);

    try
    {
        if (route->_default_file.has_value())
        {
            response = new FileResponse(target + Path(route->_default_file.value()), _connection);
            return new SendResponseTask(server, std::move(file), response);
        }
    }
    catch (const std::exception& e)
    {
        INFO("Request::into_response: " << e.what());
        if (server.map_attributes(_uri.host()).dirlist())
        {
            response = new DirectoryResponse(target, _uri.path(), _connection);
            return new SendResponseTask(server, std::move(file), response);
        }
        throw HTTPError(Status::NOT_FOUND);
    }

    if (!server.map_attributes(_uri.host()).dirlist())
        throw HTTPError(Status::FORBIDDEN);

    response = new DirectoryResponse(target, _uri.path(), _connection);
    return new SendResponseTask(server, std::move(file), response);
}

const Method& Request::method() const
{
    return _request_line.method();
}

const Header* Request::header(const string& name) const
{
    auto header = std::find_if(
        _headers.cbegin(), _headers.cend(),
        [name](const Header& header) { return header._name == name; }
    );

    return header != _headers.cend() ? &(*header) : nullptr;
}
