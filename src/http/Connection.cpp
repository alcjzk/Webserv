#include <utility>
#include <optional>
#include "File.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Reader.hpp"
#include "Connection.hpp"

Connection::Connection(File&& client, const Server& server)
    : _client(std::move(client)), _server(server)
{
}

const File& Connection::client() const&
{
    return _client;
}

File& Connection::client() &
{
    return _client;
}

File&& Connection::client() &&
{
    return std::move(_client);
}

const Server& Connection::server() const
{
    return _server;
}

const Config& Connection::config() const
{
    return _server.config();
}

std::optional<Reader>& Connection::reader()
{
    return _reader;
}

const std::optional<Reader>& Connection::reader() const
{
    return _reader;
}
