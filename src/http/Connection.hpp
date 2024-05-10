#pragma once

#include <optional>
#include <string>
#include "File.hpp"
#include "Config.hpp"
#include "Reader.hpp"

class Server;

class Connection
{
    public:
        bool _keep_alive = true;

        Connection(File&& client, const Server& server, const std::string& ip);

        const File& client() const&;
        File&       client() &;
        File&&      client() &&;

        const Server& server() const;

        const Config& config() const;

        std::optional<Reader>&       reader();
        const std::optional<Reader>& reader() const;
        const std::string&           ip() const;

    private:
        std::string           _ip;
        File                  _client;
        const Server&         _server;
        std::optional<Reader> _reader = std::nullopt;
};
