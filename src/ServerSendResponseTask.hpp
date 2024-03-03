#pragma once

#include "Task.hpp"
#include "Response.hpp"
#include "Config.hpp"

class ServerSendResponseTask : public Task
{
    public:
        virtual ~ServerSendResponseTask() override;

        ServerSendResponseTask(const Config& config, int fd, Response* response);
        ServerSendResponseTask(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask(ServerSendResponseTask&&) = delete;

        ServerSendResponseTask& operator=(const ServerSendResponseTask&) = delete;
        ServerSendResponseTask& operator=(ServerSendResponseTask&&) = delete;

        virtual void            run() override;
        virtual void            abort() override;

    private:
        Response* _response;
};
