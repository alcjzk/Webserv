#pragma once

#include "Task.hpp"
#include "Server.hpp"

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
