#pragma once

#include "Task.hpp"
#include "Server.hpp"

class AcceptTask : public Task
{
    public:
        AcceptTask(const Server& server);

        virtual void run() override;

    private:
        const Server& _server;
};
