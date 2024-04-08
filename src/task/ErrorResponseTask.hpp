#pragma once

#include "ReadTask.hpp"
#include "SendResponseTask.hpp"
#include "CompositeTask.hpp"
#include "Response.hpp"
#include "File.hpp"
#include "Server.hpp"
#include "Status.hpp"

namespace error_response_task
{
    using Connection = Response::Connection;

    struct SendState
    {
        public:
            SendResponseTask _task;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    struct ReadState
    {
        public:
            ReadTask      _task;
            File          _client;
            Connection    _connection;
            const Server& _server;
            Status        _status;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class ErrorResponseTask : public CompositeTask<ReadState, SendState>
    {
        public:
            ErrorResponseTask(
                File&& client, const Server& server, Status status,
                Connection = Connection::KeepAlive
            );
    };

} // namespace error_response_task

using ErrorResponseTask = error_response_task::ErrorResponseTask;

#include "ErrorResponseTask.tpp"
