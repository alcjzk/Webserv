#pragma once

#include <cstddef>
#include "File.hpp"
#include "ReadTask.hpp"
#include "SendResponseTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "CompositeTask.hpp"

namespace file_response_task
{
    using Connection = Response::Connection;

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
            ReadTask      _task;
            File          _client;
            const Server& _server;
            Connection    _connection;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class FileResponseTask : public CompositeTask<ReadState, SendState, ErrorState>
    {
        public:
            FileResponseTask(
                File&& file, size_t size, File&& client, const Server& server, Connection connection
            );
    };
} // namespace file_response_task

using FileResponseTask = file_response_task::FileResponseTask;

#include "FileResponseTask.tpp"
