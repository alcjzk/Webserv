#pragma once

#include "Task.hpp"
#include "Server.hpp"

class ServerAcceptTask : public Task
{
    public:
        ServerAcceptTask(const Server& server);

        virtual void run() override;

    private:
        const Server& _server;
};
