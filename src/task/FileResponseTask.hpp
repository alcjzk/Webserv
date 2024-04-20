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
} // namespace file_response_task

using FileResponseTask = file_response_task::FileResponseTask;

#include "FileResponseTask.tpp"
