#pragma once

#include "ReadTask.hpp"
#include "SendResponseTask.hpp"
#include "CompositeTask.hpp"
#include "Connection.hpp"
#include "Status.hpp"

namespace error_response_task
{
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
            ReadTask   _task;
            Connection _connection;
            Status     _status;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class ErrorResponseTask : public CompositeTask<ReadState, SendState>
    {
        public:
            ErrorResponseTask(Connection&& connection, Status status);
    };

} // namespace error_response_task

using ErrorResponseTask = error_response_task::ErrorResponseTask;

#include "ErrorResponseTask.tpp"
