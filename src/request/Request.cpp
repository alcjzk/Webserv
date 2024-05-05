#include <cstddef>
#include <fcntl.h>
#include <stdexcept>
#include <sys/fcntl.h>
#include <utility>
#include <string>
#include <cassert>
#include "FieldName.hpp"
#include "Log.hpp"
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
#include "CGICreationTask.hpp"
#include "FileResponseTask.hpp"
#include "UploadResponseTask.hpp"
#include "http.hpp"

using std::optional;
using std::string;

void Request::Builder::body(Body&& body)
{
    _body = std::move(body);
}

void Request::Builder::header(const std::string& header)
{
    if (!_headers.insert(http::parse_field(header)))
        throw HTTPError(Status::BAD_REQUEST);
}

void Request::Builder::request_line(RequestLine&& request_line)
{
    if (!request_line.http_version().is_compatible_with(Server::http_version()))
        throw HTTPError(Status::HTTP_VERSION_NOT_SUPPORTED);
    _request_line = std::move(request_line);
}

FieldMap& Request::Builder::headers()
{
    return _headers;
}

const FieldMap& Request::Builder::headers() const
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

void Request::Builder::parse_headers()
{
    try
    {
        const FieldValue* host = _headers.get(FieldName::HOST);
        if (!host)
            throw HTTPError(Status::BAD_REQUEST);

        _uri = HttpUri(_request_line.request_target(), *host);

        if (const FieldValue* connection = _headers.get(FieldName::CONNECTION))
        {
            if (**connection == "close")
                _keep_alive = false;
        }

        const FieldValue* transfer_encoding = _headers.get(FieldName::TRANSFER_ENCODING);
        const FieldValue* content_length = _headers.get(FieldName::CONTENT_LENGTH);

        if (transfer_encoding)
        {
            if (content_length)
                throw HTTPError(Status::BAD_REQUEST);
            if (_request_line.http_version() == HTTPVersion(1, 0))
                throw HTTPError(Status::BAD_REQUEST);
            if (**transfer_encoding != "chunked")
                throw HTTPError(Status::NOT_IMPLEMENTED);
            _is_chunked = true;
            bool was_erased = _headers.erase(FieldName::TRANSFER_ENCODING);
            assert(was_erased);
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
    return Request(std::move(*this));
}

Request::Request(Request::Builder&& builder)
    : _uri(std::move(builder._uri).value()), _request_line(std::move(builder._request_line)),
      _headers(std::move(builder._headers)), _body(std::move(builder._body))
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
        if (static_cast<std::string>(target).substr(static_cast<std::string>(target).size() - 3) == ".py")
            return new CGICreationTask(std::move(connection), *this, target, server.config());
        int    fd = target.open(O_RDONLY);
        size_t size = target_status->size();
        INFO("target_status->size() " << size);
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

const HttpUri& Request::uri() const
{
    return _uri;
}

const RequestLine& Request::request_line() const
{
    return _request_line;
}

const Method& Request::method() const
{
    return _request_line.method();
}

const FieldMap& Request::headers() const
{
    return _headers;
}

const Request::Body& Request::body() const&
{
    return _body;
}

Request::Body&& Request::body() &&
{
    return std::move(_body);
}
