#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <filesystem>
#include <utility>
#include "Task.hpp"
#include "Config.hpp"
#include "HTTPVersion.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Response.hpp"
#include "Routes.hpp"
#include "Reader.hpp"
#include "Request.hpp"

class Server
{
    public:
        ~Server();

        Server(const Config& config);
        Server(const Server&) = delete;
        Server(Server&&) = delete;

        Server&                  operator=(const Server&) = delete;
        Server&                  operator=(Server&&) = delete;

        int                      fd() const;
        const Route*             route(const std::string& uri_path) const;
        static const HTTPVersion http_version();

    private:
        const Config&    _config;
        const char*      _port;
        struct addrinfo* _address_info;
        int              _fd;
        Routes           _routes;
};

class ServerSendResponseTask : public Task
{
    public:
        virtual ~ServerSendResponseTask() override;

        ServerSendResponseTask(int fd, Response* response);
        ServerSendResponseTask(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask(ServerSendResponseTask&&) = delete;

        ServerSendResponseTask& operator=(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask& operator=(ServerSendResponseTask&&) = delete;

        virtual void            run() override;

    private:
        Response* _response;
};

class ServerReceiveRequestTask : public Task
{
    public:
        virtual ~ServerReceiveRequestTask() override;

        ServerReceiveRequestTask(const Server& server, int fd);
        ServerReceiveRequestTask(const ServerReceiveRequestTask&) = delete;
        ServerReceiveRequestTask(ServerReceiveRequestTask&&) = delete;

        ServerReceiveRequestTask& operator=(const ServerReceiveRequestTask&) = delete;
        ServerReceiveRequestTask& operator=(ServerReceiveRequestTask&&) = delete;

        virtual void              run() override;

    private:
        typedef enum Expect
        {
            REQUEST_LINE,
            HEADERS
        } Expect;

        // State impl
        void                receive_start_line();
        void                receive_headers();
        // TODO: void receive_body()

        // Util
        void                fill_buffer();
        char*               buffer_head();
        size_t              buffer_size_available();

        // TODO: Use value from config + expanding buffersize?
        static const size_t _header_buffer_size = 4096;
        Expect              _expect;
        size_t              _bytes_received_total;
        std::vector<char>   _buffer;
        Reader              _reader;
        Request             _request;
        bool                _is_partial_data;
        const Server&       _server;
};

class ServerAcceptTask : public Task
{
    public:
        virtual ~ServerAcceptTask() override;

        ServerAcceptTask(const Server& server);

        ServerAcceptTask(const ServerAcceptTask&) = delete;
        ServerAcceptTask(ServerAcceptTask&&) = delete;

        ServerAcceptTask& operator=(const ServerAcceptTask&) = delete;
        ServerAcceptTask& operator=(ServerAcceptTask&&) = delete;

        virtual void      run() override;

    private:
        const Server& _server;
};
