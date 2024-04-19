#pragma once

#include <optional>
#include "File.hpp"
#include "Config.hpp"
#include "Reader.hpp"

class Server;

class Connection
{
    public:
        bool _keep_alive = true;

        Connection(File&& client, const Server& server);

        const File& client() const&;
        File&       client() &;
        File&&      client() &&;

        const Server& server() const;

        const Config& config() const;

        std::optional<Reader>&       reader();
        const std::optional<Reader>& reader() const;

    private:
        File                  _client;
        const Server&         _server;
        std::optional<Reader> _reader = std::nullopt;
};
