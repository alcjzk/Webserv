#include <algorithm>
#include <cstddef>
#include <fcntl.h>
#include <cctype>
#include <stdexcept>
#include <sys/fcntl.h>
#include <utility>
#include <string>
#include <cassert>
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
#include "UploadResponseTask.hpp"

using std::optional;
using std::string;

void Request::Builder::body(Body&& body)
{
    _body = std::move(body);
}

void Request::Builder::header(Header&& header)
{
    if (!_headers.emplace(header._name, header._value).second)
        throw HTTPError(Status::BAD_REQUEST);
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

optional<ContentLength> Request::Builder::content_length() const
{
    return _content_length;
}

bool Request::Builder::is_chunked() const
{
    return _is_chunked;
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

void Request::Builder::parse_headers()
{
    try
    {
        string* host = header_by_key("host");
        if (!host)
            throw HTTPError(Status::BAD_REQUEST);
        to_lower_in_place(*host);

        _uri = HttpUri(_request_line.request_target(), *host);

        if (string* connection = header_by_key("connection"))
        {
            to_lower_in_place(*connection);
            if (*connection == "close")
                _keep_alive = false;
        }

        string*       transfer_encoding = header_by_key("transfer-encoding");
        const string* content_length = header_by_key("content-length");

        if (transfer_encoding)
        {
            if (content_length)
                throw HTTPError(Status::BAD_REQUEST);
            if (_request_line.http_version() == HTTPVersion(1, 0))
                throw HTTPError(Status::BAD_REQUEST);
            to_lower_in_place(*transfer_encoding);
            if (*transfer_encoding != "chunked")
                throw HTTPError(Status::NOT_IMPLEMENTED);
            _is_chunked = true;
            size_t erased_count = _headers.erase("transfer-encoding");
            assert(erased_count == 1);
        }
        else if (content_length)
            _content_length = ContentLength(*content_length);
    }
    catch (const std::runtime_error& error)
    {
        WARN("Request::Builder::build(): " << error.what());
        throw HTTPError(Status::BAD_REQUEST);
    }
}

Request Request::Builder::build() &&
{
    return Request(
        std::move(*_uri), std::move(_request_line), std::move(_headers), std::move(_body)
    );
}

void Request::Builder::to_lower_in_place(string& value)
{
    auto to_lower = [](unsigned char c) { return std::tolower(c); };
    (void)std::transform(value.begin(), value.end(), value.begin(), to_lower);
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

    if (_request_line.method() == Method::POST && route->_type == Route::UPLOAD)
    {
        return new UploadResponseTask(std::move(connection), *this, route->_fs_path, target);
    }

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

string* Request::header_by_key(const string& key)
{
    auto entry = _headers.find(key);
    return entry != _headers.end() ? &entry->second : nullptr;
}

const string* Request::header_by_key(const string& key) const
{
    const auto entry = _headers.find(key);
    return entry != _headers.end() ? &entry->second : nullptr;
}
