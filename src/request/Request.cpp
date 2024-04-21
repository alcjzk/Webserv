#include <algorithm>
#include <cstddef>
#include <fcntl.h>
#include <cctype>
#include <stdexcept>
#include <sys/fcntl.h>
#include <utility>
#include <string>
#include "Log.hpp"
#include "Header.hpp"
#include "RequestLine.hpp"
#include "Method.hpp"
#include "Response.hpp"
#include "Status.hpp"
#include "Server.hpp"
#include "SendResponseTask.hpp"
#include "DirectoryResponse.hpp"
#include "RedirectionResponse.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "HttpUri.hpp"
#include "File.hpp"
#include "Task.hpp"
#include "Route.hpp"
#include "Path.hpp"
#include "ContentLength.hpp"
#include "Connection.hpp"
#include "FileResponseTask.hpp"

using std::string;

void Request::Builder::body(Body&& body)
{
    _body = std::move(body);
}

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
                _keep_alive = false;
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

ContentLength Request::Builder::content_length() const
{
    if (const auto& it = _headers.find("content-length"); it != _headers.end())
    {
        return ContentLength(it->second);
    }
    return ContentLength(0);
}

string* Request::Builder::header_by_key(const string& key)
{
    auto entry = _headers.find(key);
    return entry != _headers.end() ? &entry->second : nullptr;
}

const string* Request::Builder::header_by_key(const string& key) const
{
    const auto entry = _headers.find(key);
    return entry != _headers.end() ? &entry->second : nullptr;
}

Request Request::Builder::build() &&
{
    if (!_uri)
        throw HTTPError(Status::BAD_REQUEST);
    return Request(
        std::move(*_uri), std::move(_request_line), std::move(_headers), std::move(_body)
    );
}

Request::Request(HttpUri&& uri, RequestLine&& request_line, Headers&& headers, Body&& body)
    : _uri(std::move(uri)), _request_line(std::move(request_line)), _headers(std::move(headers)),
      _body(std::move(body))
{
}

Task* Request::process(Connection&& connection)
{
    const Server& server = connection.server();

    const Route* route = server.route(_uri.path(), _uri.host());

    if (!route)
        throw HTTPError(Status::NOT_FOUND);

    if (route->_type == Route::REDIRECTION)
    {
        Response* response = new RedirectionResponse(route->_redir.value());
        response->_keep_alive = connection._keep_alive;
        return new SendResponseTask(std::move(connection), response);
    }

    Path target = route->map(_uri.path());
    if (!route->method_get())
        throw HTTPError(Status::FORBIDDEN);

    auto target_status = target.status();
    if (!target_status)
        throw HTTPError(Status::NOT_FOUND);

    if (target_status->is_regular())
    {
        // TODO: Open is assumed to succeed here
        int    fd = target.open(O_RDONLY);
        size_t size = target_status->size();
        return new FileResponseTask(std::move(connection), fd, size);
    }

    if (!target_status->is_directory())
        throw HTTPError(Status::FORBIDDEN);

    if (route->_default_file)
    {
        Path default_file = target + Path(route->_default_file.value());
        auto status = default_file.status();
        if (status.has_value() && status->is_regular())
        {
            // TODO: Expected behavior when default file is set and exists but is not a regular
            // file?
            // TODO: Open is assumed to succeed here
            return new FileResponseTask(
                std::move(connection), default_file.open(O_RDONLY), status->size()
            );
        }
    }

    if (!server.map_attributes(_uri.host()).dirlist())
        throw HTTPError(Status::FORBIDDEN);

    Response* response = new DirectoryResponse(target, _uri.path());
    response->_keep_alive = connection._keep_alive;
    return new SendResponseTask(std::move(connection), response);
}

const Method& Request::method() const
{
    return _request_line.method();
}

const Request::Headers& Request::headers() const
{
    return _headers;
}
