#include <algorithm>
#include <fcntl.h>
#include <cctype>
#include <stdexcept>
#include <exception>
#include <sys/fcntl.h>
#include <utility>
#include <string>
#include <vector>
#include "Log.hpp"
#include "Header.hpp"
#include "RequestLine.hpp"
#include "Method.hpp"
#include "Response.hpp"
#include "Status.hpp"
#include "Server.hpp"
#include "FileResponse.hpp"
#include "DirectoryResponse.hpp"
#include "RedirectionResponse.hpp"
#include "SendResponseTask.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "HttpUri.hpp"
#include "File.hpp"
#include "Task.hpp"
#include "Route.hpp"
#include "Path.hpp"
#include "FileResponseTask.hpp"

using std::string;
using std::vector;

void Request::Builder::header(Header&& header)
{
    try
    {
        if (header._name == "host")
        {
            _uri = HttpUri(_request_line.request_target(), header._value);
        }
        else if (header._name == "connection")
        {
            (void)std::transform(
                header._value.begin(), header._value.end(), header._value.begin(), toupper
            );
            if (header._value == "close")
            {
                _connection = Connection::Close;
            }
        }
        if (!_headers.emplace(header._name, header._value).second)
            throw HTTPError(Status::BAD_REQUEST);
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

const Request::Headers& Request::Builder::headers() const
{
    return _headers;
}

Request Request::Builder::build() &&
{
    if (!_uri)
        throw HTTPError(Status::BAD_REQUEST);
    if (const auto it = _headers.find("content-length"); it != _headers.end())
    {
        const auto [name, value] = *it;

        // TODO: Validate content-length
        size_t content_length = std::stoull(value);

        if (content_length != 0)
        {
            _body.resize(content_length);
        }
    }
    return Request(
        std::move(*_uri), _connection, std::move(_request_line), std::move(_headers),
        std::move(_body)
    );
}

Request::Request(
    HttpUri&& uri, Connection connection, RequestLine&& request_line, Headers&& headers, Body&& body
)
    : _uri(std::move(uri)), _connection(connection), _request_line(std::move(request_line)),
      _headers(std::move(headers)), _body(std::move(body))
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

    auto target_status = target.status();
    if (!target_status)
        throw HTTPError(Status::NOT_FOUND);

    if (target_status->is_regular())
    {
        int    fd = target.open(O_RDONLY);
        size_t size = target_status->size();
        return new FileResponseTask(fd, size, std::move(file), server, _connection);
    }

    if (!target_status->is_directory())
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

const Request::Headers& Request::headers() const
{
    return _headers;
}
