#pragma once

#include "BasicTask.hpp"
#include "Server.hpp"

class AcceptTask : public BasicTask
{
    public:
        AcceptTask(const Server& server);

        virtual void run() override;

    private:
        const Server& _server;
};
