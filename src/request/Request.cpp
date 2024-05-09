#include <fcntl.h>
#include <stdexcept>
#include <sys/fcntl.h>
#include <utility>
#include <string>
#include <cassert>
#include <memory>
#include <cstdio>
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
using std::unique_ptr;

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

        const FieldValue* connection = _headers.get(FieldName::CONNECTION);
        if (_request_line.http_version() == HTTPVersion(1, 0))
        {
            if (!connection || connection->eq_case_insensitive("keep-alive"))
                _keep_alive = false;
        }
        else if (connection && connection->eq_case_insensitive("close"))
        {
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
            if (!transfer_encoding->eq_case_insensitive("chunked"))
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
      _headers(std::move(builder._headers)), _body(std::move(builder._body)),
      _keep_alive(builder._keep_alive)
{
}

Task* Request::process(Connection&& connection)
{
    const Server& server = connection.server();
    connection._keep_alive = _keep_alive;

    const Route* route = server.route(_uri.path(), _uri.host());

    if (!route)
        throw HTTPError(Status::NOT_FOUND);

    if (!route->allowed_methods().test(method()))
        throw HTTPError(Status::METHOD_NOT_ALLOWED);

    if (route->_type == Route::REDIRECTION)
    {
        unique_ptr<Response> response =
            std::make_unique<RedirectionResponse>(route->_redir.value());
        response->keep_alive = connection._keep_alive;
        return new SendResponseTask(std::move(connection), std::move(response));
    }

    Path target = route->map(_uri.path());

    auto target_status = target.status();
    if (!target_status)
        throw HTTPError(Status::NOT_FOUND);

    if (target_status->is_regular())
    {
        if (static_cast<std::string>(target).substr(static_cast<std::string>(target).size() - 3) == ".py")
            return new CGICreationTask(std::move(connection), *this, target, (Config&)server.config());
        if (method() == Method::Delete)
        {
            if (std::remove(static_cast<string>(target).c_str()) != 0)
                throw HTTPError(Status::INTERNAL_SERVER_ERROR);
            else
            {
                unique_ptr<Response> response = std::make_unique<Response>(Status::OK);
                response->keep_alive = connection._keep_alive;
                return new SendResponseTask(std::move(connection), std::move(response));
            }
        }
        auto fd = target.open(O_RDONLY | O_NONBLOCK | O_CLOEXEC);
        if (!fd)
            throw HTTPError(Status::NOT_FOUND);
        return new FileResponseTask(std::move(connection), fd.value(), target_status->size());
    }

    if (!target_status->is_directory())
        throw HTTPError(Status::FORBIDDEN);

    if (_request_line.method() == Method::Post && route->_type == Route::UPLOAD)
    {
        return new UploadResponseTask(std::move(connection), *this, route->_fs_path, target);
    }

    if (route->_default_file)
    {
        Path default_file = target + Path(route->_default_file.value());
        if (auto status = default_file.status())
        {
            if (status->is_regular())
            {
                if (auto fd = default_file.open(O_RDONLY | O_NONBLOCK | O_CLOEXEC))
                    return new FileResponseTask(std::move(connection), fd.value(), status->size());
            }
            WARN("default file `" << default_file << "` exists but could not be opened");
        }
    }

    if (!server.map_attributes(_uri.host()).dirlist())
        throw HTTPError(Status::FORBIDDEN);

    unique_ptr<Response> response = std::make_unique<DirectoryResponse>(target, _uri.path());
    response->keep_alive = connection._keep_alive;
    return new SendResponseTask(std::move(connection), std::move(response));
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
