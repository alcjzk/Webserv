#pragma once

#include <memory>
#include "Response.hpp"
#include "Connection.hpp"
#include "BasicTask.hpp"

class SendResponseTask : public BasicTask
{
    public:
        SendResponseTask(Connection&& connection, Response* response);

        virtual void run() override;
        virtual void abort() override;
        virtual int  fd() const override;

    private:
        Connection                _connection;
        std::unique_ptr<Response> _response;
};
