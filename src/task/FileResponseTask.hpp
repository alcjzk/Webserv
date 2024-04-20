#pragma once

#include <cstddef>
#include "File.hpp"
#include "ReadTask.hpp"
#include "SendResponseTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Connection.hpp"
#include "CompositeTask.hpp"

namespace file_response_task
{
    class SendState
    {
        public:
            SendResponseTask _task;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class ErrorState
    {
        public:
            ErrorResponseTask _task;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class ReadState
    {
        public:
            ReadTask   _task;
            Connection _connection;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class FileResponseTask : public CompositeTask<ReadState, SendState, ErrorState>
    {
        public:
            FileResponseTask(Connection&& connection, File&& file, size_t size);
    };

    // Template impls

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

using FileResponseTask = file_response_task::FileResponseTask;
