#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "Task.hpp"
#include "Config.hpp"
#include "HTTPVersion.hpp"
#include "RequestLine.hpp"
#include "Runtime.hpp"
#include "Response.hpp"

class Server
{
    public:
        Server(const Config &config);
        ~Server();

        int fd();

        static const HTTPVersion http_version();

    private:
        Server(const Server&);
        Server& operator=(const Server&);

        const Config            &_config;
        const char*             _port;
        struct addrinfo         *_address_info;
        int                     _fd;
};

class ServerSendResponseTask : public Task
{
    public:
        ServerSendResponseTask(int fd, Response* response);

        virtual ~ServerSendResponseTask();
        virtual void run();

    private:
        ServerSendResponseTask(const ServerSendResponseTask&);
        ServerSendResponseTask& operator=(const ServerSendResponseTask&);

        Response* _response;
};

class ServerReceiveRequestTask : public Task
{
    public:
        ServerReceiveRequestTask(int fd);

        virtual ~ServerReceiveRequestTask();
        virtual void run();

    private:
        typedef enum Expect
        {
            REQUEST_LINE,
            HEADERS
        } Expect;

        ServerReceiveRequestTask(const ServerReceiveRequestTask&);
        ServerReceiveRequestTask& operator=(const ServerReceiveRequestTask&);

        // State impl
        void        receive_start_line();
        void        receive_headers();
        // TODO: void receive_body()

        // Util
        void        fill_buffer();
        char*    buffer_head();
        size_t      buffer_size_available();

        // TODO: Use value from config + expanding buffersize?
        static const size_t  _header_buffer_size = 4096;
        Expect               _expect;
        size_t               _bytes_received_total;
        std::vector<char> _buffer;
        Reader               _reader;
        RequestLine          _request_line;
};

class ServerAcceptTask : public Task
{
    public:
        ServerAcceptTask(Server& server);

        virtual ~ServerAcceptTask();
        virtual void run();

    private:
        ServerAcceptTask(const ServerAcceptTask&);
        ServerAcceptTask& operator=(const ServerAcceptTask&);

        Server& _server;
};
