#pragma once

#include "CompositeTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Request.hpp"
#include "Path.hpp"
#include "SendResponseTask.hpp"
#include "Connection.hpp"
#include "CGIReadTask.hpp"
#include "CGIWriteTask.hpp"
#include "Log.hpp"
#include <fcntl.h>

namespace cgi_creation_task
{

    struct ReadState
    {
        public:
            CGIReadTask       _task;
            Connection        _connection;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    struct WriteState
    {
        public:
            CGIWriteTask _task;
            pid_t        _pid;
            Connection  _connection;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    struct SendState
    {
        public:
            SendResponseTask _task;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    struct ErrorState
    {
        public:
            ErrorResponseTask _task;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class CGICreationTask : public CompositeTask<ReadState, WriteState, SendState, ErrorState>
    {
        public:
            CGICreationTask(
                Connection&& connection, Request& request, const Path& uri, const Config& config
            );

        private:
            std::vector<char*> _environment;
            int _pipe_fd[2];
    };

    template <typename Parent>
    void WriteState::on_complete(Parent& parent)
    {
        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        ReadState read_state{
            CGIReadTask(File(_task.read_end()), _task.config(), _pid),
            std::move(_connection),
        };
        parent.state(std::move(read_state));
    }

    template <typename Parent>
    void ReadState::on_complete(Parent& parent)
    {
        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        INFO("READ TASK IS COMPLETE!");
        Response* response = new Response(Status::OK);
        response->_keep_alive = _connection._keep_alive;
        response->body(std::move(_task.buffer()));

        SendState send_state{
            SendResponseTask(std::move(_connection), response),
        };
        parent.state(std::move(send_state));
    }

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

} // namespace cgi_creation_task

using CGICreationTask = cgi_creation_task::CGICreationTask;
