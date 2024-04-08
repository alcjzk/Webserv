#pragma once

#include <memory>
#include "Response.hpp"
#include "Server.hpp"
#include "BasicTask.hpp"
#include "File.hpp"

class SendResponseTask : public BasicTask
{
    public:
        SendResponseTask(const Server& server, File&& file, Response* response);

        virtual void run() override;
        virtual void abort() override;

    private:
        std::unique_ptr<Response> _response;
        const Server&             _server;
};
