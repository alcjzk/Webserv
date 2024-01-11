#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <cassert>
#include <sstream>
#include <errno.h>
#include "http.hpp"
#include "Response.hpp"

Response::~Response()
{
    delete _buffer;
    _buffer = nullptr;
}

Response::Response(Status status)
    : _status(status), _buffer(nullptr), _size(0), _size_remaining(0), _is_built(false)
{
}

bool Response::send(int fd)
{
    ssize_t bytes_sent;
    size_t  offset;

    if (!_is_built)
        build();
    assert(_is_built);
    offset = _size - _size_remaining;
    bytes_sent = ::send(fd, &_buffer[offset], _size_remaining, 0);
    if (bytes_sent == -1)
        throw std::runtime_error(strerror(errno));
    assert(bytes_sent >= 0);
    _size_remaining -= bytes_sent;
    if (_size_remaining == 0)
        return true;
    return false;
}

void Response::header(const Header& header)
{
    _headers.push_back(header);
}

void Response::header(Header&& header)
{
    _headers.push_back(std::move(header));
}

const std::vector<Header>& Response::headers() const
{
    return _headers;
}

void Response::body(std::vector<char>&& body)
{
    content_length(body.size());
    _body = std::move(body);
}

void Response::body(const std::vector<char>& body)
{
    content_length(body.size());
    _body = body;
}

const std::vector<char>& Response::body() const
{
    return _body;
}

void Response::content_length(size_t content_length)
{
    header(Header("Content-Length", std::to_string(content_length)));
}

void Response::build()
{
    std::stringstream   headers_stream;
    std::streambuf*     headers_rdbuf;
    size_t              headers_size;
    size_t              file_size;
    std::vector<char>   body;
    std::vector<Header> headers;

    headers.swap(_headers);
    body.swap(_body);

    headers_stream << _status.as_status_line() << http::CRLF;
    for (const auto& header : headers)
    {
        headers_stream << header << http::CRLF;
    }
    headers_stream << http::CRLF;

    headers_rdbuf = headers_stream.rdbuf();
    headers_size = headers_rdbuf->pubseekoff(0, headers_stream.end, headers_stream.in);

    _size = headers_size + body.size();
    _size_remaining = _size;
    _buffer = new char[_size];

    headers_rdbuf->pubseekpos(0, headers_stream.in);
    headers_rdbuf->sgetn(_buffer, headers_size);

    std::memcpy(&_buffer[headers_size], body.data(), body.size());
    _is_built = true;
}
