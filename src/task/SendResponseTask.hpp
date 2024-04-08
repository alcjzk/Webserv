#pragma once

#include "Task.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "File.hpp"

class SendResponseTask : public Task
{
    public:
        virtual ~SendResponseTask() override;

        SendResponseTask(const Server& server, File&& file, Response* response);
        SendResponseTask(const SendResponseTask&) = delete;
        SendResponseTask(SendResponseTask&&) = delete;

        SendResponseTask& operator=(const SendResponseTask&) = delete;
        SendResponseTask& operator=(SendResponseTask&&) = delete;

        virtual void run() override;
        virtual void abort() override;

    private:
        Response*     _response; // TODO: Replace with unique_ptr
        const Server& _server;
};
