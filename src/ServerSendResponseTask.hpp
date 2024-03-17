#pragma once

#include "Task.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "File.hpp"

class ServerSendResponseTask : public Task
{
    public:
        virtual ~ServerSendResponseTask() override;

        ServerSendResponseTask(const Server& server, File&& file, Response* response);
        ServerSendResponseTask(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask(ServerSendResponseTask&&) = delete;

        ServerSendResponseTask& operator=(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask& operator=(ServerSendResponseTask&&) = delete;

        virtual void            run() override;
        virtual void            abort() override;

    private:
        Response*     _response; // TODO: Replace with unique_ptr
        const Server& _server;
};
