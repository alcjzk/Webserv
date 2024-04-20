#pragma once

#include <utility>
#include "Response.hpp"
#include "Status.hpp"
#include "ErrorResponseTask.hpp"
#include "SendResponseTask.hpp"
#include "FileResponseTask.hpp"

namespace file_response_task
{
    template <typename Parent>
    void SendState::on_complete(Parent& parent)
    {
        parent.set_complete();
    }

    template <typename Parent>
    void ErrorState::on_complete(Parent& parent)
    {
        parent.set_complete();
    }

    template <typename Parent>
    void ReadState::on_complete(Parent& parent)
    {
        Response* response;

        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        response = new Response(Status::OK);
        response->_keep_alive = _connection._keep_alive;
        response->body(std::move(_task).buffer());

        SendState send_state{
            SendResponseTask(std::move(_connection), response),
        };
        parent.state(std::move(send_state));
    }
} // namespace file_response_task
