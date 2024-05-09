#include <cctype>
#include <limits>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <cassert>
#include <stdexcept>
#include <string>
#include <optional>
#include <exception>
#include <errno.h>
#include <sys/socket.h>
#include "Status.hpp"
#include "Reader.hpp"
#include "Request.hpp"
#include "ReceiveRequestTask.hpp"
#include "ErrorResponseTask.hpp"
#include "SendResponseTask.hpp"
#include "TimeoutResponse.hpp"
#include "HTTPError.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Error.hpp"
#include "Log.hpp"
#include <utility>
#include "Buffer.hpp"
#include "BasicTask.hpp"
#include "File.hpp"
#include "Connection.hpp"
#include "Task.hpp"

using std::optional;
using std::string;
using Seconds = Task::Seconds;

ReceiveRequestTask::ReceiveRequestTask(Connection&& connection)
    : BasicTask(File(), WaitFor::Readable), _connection(std::move(connection)),
      _expire_time(_connection.config().keepalive_timeout())
{
    if (!_connection.reader())
    {
        _connection.reader().emplace(_connection.config().header_buffsize());
        _is_partial_data = true;
    }
}

void ReceiveRequestTask::fill_buffer()
{
    Reader& reader = _connection.reader().value();

    if (reader.buffer().unfilled_size() == 0)
    {
        assert(
            _expect == Expect::Headers || _expect == Expect::RequestLine ||
            _expect == Expect::ChunkSize
        );
        if (!reader.grow(RequestLine::MAX_LENGTH))
        {
            if (reader.is_aligned())
                throw HTTPError(Status::CONTENT_TOO_LARGE);
            realign_reader();
        }
    }

    ssize_t bytes_received =
        recv(_connection.client(), reader.buffer().unfilled(), reader.buffer().unfilled_size(), 0);
    if (bytes_received == 0)
    {
        throw Error(Error::CLOSED);
    }
    else if (bytes_received == -1)
    {
        throw std::runtime_error("recv failed");
    }
    reader.buffer().advance(bytes_received);
    _is_partial_data = false;
}

void ReceiveRequestTask::receive_start_line()
{
    Reader& reader = _connection.reader().value();
    try
    {
        Request::Builder& builder = _builder.value();
        reader.trim_empty_lines();
        builder.request_line(reader.line(RequestLine::MAX_LENGTH).value());
        INFO(builder._request_line);
        _expect = Expect::Headers;
        _expire_time = _connection.config().client_header_timeout();
    }
    catch (const std::bad_optional_access&)
    {
        _is_partial_data = true;
    }
    catch (const Reader::LineLimitError&)
    {
        throw HTTPError(Status::URI_TOO_LONG);
    }
}

void ReceiveRequestTask::receive_headers()
{
    Reader& reader = _connection.reader().value();
    while (true)
    {
        auto line = reader.line();
        if (!line)
        {
            _is_partial_data = true;
            return;
        }
        if (line->empty())
        {
            INFO("End of headers");
            _builder->parse_headers();

            if (_builder->is_chunked())
            {
                realign_reader();
                _expect = Expect::ChunkSize;
                _expire_time = _connection.config().client_body_timeout();
                return;
            }
            else if (auto content_length = _builder->content_length())
            {
                if (*content_length > _connection.config().body_size())
                    throw HTTPError(Status::CONTENT_TOO_LARGE);
                if (*content_length != 0)
                {
                    INFO("expecting " << *content_length << " bytes of content");

                    // If the headers buffer has more content remaining than the body size, the body
                    // can be split out from the buffer directly. Otherwise the headers buffer can
                    // be assumed to only contain body content.
                    if (reader.unread_size() <= *content_length)
                    {
                        // Headers buffer can only contain body content.
                        Buffer body_buffer(*content_length);
                        std::copy(reader.begin(), reader.end(), body_buffer.unfilled());
                        body_buffer.advance(reader.unread_size());
                        reader.buffer(std::move(body_buffer));
                        _expire_time = _connection.config().client_body_timeout();
                        _expect = Expect::Body;
                        return;
                    }
                    // Headers buffer already contains full body content.
                    _builder->body(reader.read_exact(*content_length));
                }
            }
            realign_reader();
            Request request = std::exchange(_builder, std::nullopt).value().build();
            Runtime::enqueue(request.process(std::move(_connection)));
            _is_complete = true;
            return;
        }
        _builder->header(*line);
    }
}

void ReceiveRequestTask::receive_chunk_size()
{
    try
    {
        Reader& reader = _connection.reader().value();

        auto line = reader.line(CHUNKED_SIZE_LINE_MAX_LENGTH);
        if (!line)
        {
            _is_partial_data = true;
            return;
        }
        trim_chunk_ext(*line);
        if (!is_chunk_size(*line))
            throw HTTPError(Status::BAD_REQUEST);
        _chunk_size = std::stoull(*line, nullptr, 16);
        INFO("expecting chunk with size " << _chunk_size);
        if (_chunk_size == 0)
        {
            _expect = Expect::LastChunk;
            return;
        }

        if (std::numeric_limits<size_t>::max() - _chunked_body.size() < _chunk_size)
            throw HTTPError(Status::CONTENT_TOO_LARGE);
        size_t new_size = _chunked_body.size() + _chunk_size;
        if (new_size > _connection.config().body_size())
        {
            _connection._keep_alive = false;
            throw HTTPError(Status::CONTENT_TOO_LARGE);
        }
        _chunked_position = _chunked_body.size();
        _chunked_body.resize(new_size);
        reader.reserve(_chunk_size);
        _expect = Expect::Chunk;
    }
    catch (const std::runtime_error& error)
    {
        // Should be thrown by reader.line
        WARN("ReceiveRequestTask::receive_chunk_size(): " << error.what());
        throw HTTPError(Status::BAD_REQUEST);
    }
    catch (const std::exception& error)
    {
        // stoull
        WARN("ReceiveRequestTask::receive_chunk_size(): " << error.what());
        throw HTTPError(Status::CONTENT_TOO_LARGE);
    }
}

void ReceiveRequestTask::receive_chunk()
{
    Reader& reader = _connection.reader().value();
    if (!reader.read_exact_into(_chunk_size, _chunked_body.begin() + _chunked_position))
    {
        _is_partial_data = true;
        return;
    }
    realign_reader();
    _expect = Expect::ChunkSize;
}

void ReceiveRequestTask::receive_last_chunk()
{
    Reader& reader = _connection.reader().value();

    while (auto line = reader.line())
    {
        if (!line)
        {
            _is_partial_data = true;
            return;
        }
        if (!line->empty())
            continue;

        realign_reader();
        _builder->body(std::move(_chunked_body));
        INFO("received chunked content of size " << _builder->_body.size());
        _connection.reader().reset();
        Request request = std::exchange(_builder, std::nullopt).value().build();
        Runtime::enqueue(request.process(std::move(_connection)));
        _is_complete = true;
        return;
    }
    _is_partial_data = true;
}

void ReceiveRequestTask::realign_reader()
{
    Reader& reader = _connection.reader().value();
    if (reader.is_empty())
        reader.buffer().clear();
    else
    {
        assert(reader.begin() != reader.buffer().begin());
        reader.buffer().replace(reader.begin(), reader.end());
    }
    reader.rewind();
}

void ReceiveRequestTask::receive_body()
{
    Reader& reader = _connection.reader().value();
    if (!reader.buffer().is_full())
    {
        _is_partial_data = true;
        return;
    }
    _builder->body(std::move(reader).buffer().container());
    _connection.reader().reset();
    Request request = std::exchange(_builder, std::nullopt).value().build();
    Runtime::enqueue(request.process(std::move(_connection)));
    _is_complete = true;
}

void ReceiveRequestTask::run()
{
    try
    {
        if (_is_partial_data)
        {
            fill_buffer();
        }
        while (!_is_partial_data && !_is_complete)
        {
            switch (_expect)
            {
                case Expect::RequestLine:
                    receive_start_line();
                    continue;
                case Expect::Headers:
                    receive_headers();
                    continue;
                case Expect::LastChunk:
                    receive_last_chunk();
                    continue;
                case Expect::ChunkSize:
                    receive_chunk_size();
                    continue;
                case Expect::Chunk:
                    receive_chunk();
                    continue;
                case Expect::Body:
                    receive_body();
                    continue;
                default:
                    assert(false);
            }
        }
    }
    catch (const Error& error)
    {
        assert(error == Error::CLOSED);
        WARN(error.what());
        disable_linger();
        _is_complete = true;
    }
    catch (const HTTPError& error)
    {
        WARN("ReceiveRequestTask::run():" << _connection.client() << ":" << error.what());
        auto status_code = error.status().code();
        if (status_code >= 400 && status_code != Status::NOT_FOUND &&
            status_code != Status::CONFLICT)
            _connection._keep_alive = false;
        Runtime::enqueue(new ErrorResponseTask(std::move(_connection), error.status()));
        _is_complete = true;
    }
    catch (const std::exception& error)
    {
        ERR(error.what());
        _is_complete = true;
    }
}

void ReceiveRequestTask::abort()
{
    INFO("ReceiveRequestTask for fd " << _connection.client() << " timed out");
    _is_complete = true;
    Runtime::enqueue(
        new SendResponseTask(std::move(_connection), std::make_unique<TimeoutResponse>())
    );
}

int ReceiveRequestTask::fd() const
{
    return _connection.client();
}

optional<Seconds> ReceiveRequestTask::expire_time() const
{
    return _expire_time;
}

void ReceiveRequestTask::disable_linger()
{
    struct linger linger;

    linger.l_onoff = 1;
    linger.l_linger = 0;
    if (setsockopt(_connection.client(), SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) == -1)
    {
        WARN("failed to disable linger for fd `" << _connection.client() << "`");
    }
}

void ReceiveRequestTask::trim_chunk_ext(string& value)
{
    const char CHUNK_EXT_DELIM = ';';

    auto delim = std::find(value.begin(), value.end(), CHUNK_EXT_DELIM);
    if (delim != value.end())
    {
        (void)value.erase(delim, value.end());
    }
}

bool ReceiveRequestTask::is_chunk_size(const std::string& value)
{
    if (value.empty())
        return false;
    auto not_hex = [](unsigned char c) { return !std::isxdigit(c); };
    if (std::any_of(value.begin(), value.end(), not_hex))
        return false;
    return true;
}
