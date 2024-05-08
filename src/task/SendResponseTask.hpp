#pragma once

#include <memory>
#include <optional>
#include "Response.hpp"
#include "Connection.hpp"
#include "BasicTask.hpp"

class SendResponseTask : public BasicTask
{
    public:
        SendResponseTask(Connection&& connection, std::unique_ptr<Response>&& response);

        virtual void                   run() override;
        virtual void                   abort() override;
        virtual int                    fd() const override;
        virtual std::optional<Seconds> expire_time() const override;

    private:
        Connection                _connection;
        std::unique_ptr<Response> _response;
        Seconds                   _expire_time;
};
