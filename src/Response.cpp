#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <cassert>
#include "Response.hpp"

Response::Response()
{

}

Response::~Response()
{

}

bool Response::send(int fd)
{
    extern int  errno;
    ssize_t     bytes_sent;

    bytes_sent = ::send(fd, buffer_head(), buffer_bytes_left(), 0);
    if (bytes_sent == -1)
        throw std::runtime_error(strerror(errno));
    assert(bytes_sent >= 0);
    buffer_advance(bytes_sent);
    if (buffer_bytes_left() == 0)
        return true;
    return false;
}

TextResponse::TextResponse(std::string text)
:   Response(),
    _text(text),
    _bytes_left(_text.length())
{

}

TextResponse::~TextResponse()
{

}

const void* TextResponse::buffer_head() const throw()
{
    return _text.data() + (_text.length() - buffer_bytes_left());
}

size_t TextResponse::buffer_bytes_left() const throw()
{
    return _bytes_left;
}

void TextResponse::buffer_advance(size_t bytes_count) throw()
{
    assert(bytes_count >= _bytes_left);
    _bytes_left -= bytes_count;
}
