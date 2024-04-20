#pragma once

#include <utility>
#include <fcntl.h>
#include "ReadTask.hpp"
#include "Response.hpp"
#include "SendResponseTask.hpp"
#include "ErrorResponseTask.hpp"

namespace error_response_task
{
    template <typename Parent>
    void SendState::on_complete(Parent& parent)
    {
        parent.set_complete();
    }

    template <typename Parent>
    void ReadState::on_complete(Parent& parent)
    {
        Response* response = new Response(_status);
        response->_keep_alive = _connection._keep_alive;

        if (!_task.is_error())
            response->body(std::move(_task).buffer());

        SendState send_state{SendResponseTask(std::move(_connection), response)};
        parent.state(std::move(send_state));
    }
} // namespace error_response_task
