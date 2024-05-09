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
                Connection&& connection, Request& request, const Path& uri, Config& config, std::string executable
            );

        private:
            std::vector<char*> _environment;
            int _pipe_fd[2];
            void setup_environment();
    };

    template <typename Parent>
    void WriteState::on_complete(Parent& parent)
    {
        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;
            int exit_status;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        ReadState read_state{
            CGIReadTask(_task.write_end(), _task.config(), std::move(_task).take_pid()),
            std::move(_connection),
        };
        parent.state(std::move(read_state));
    }

    template <typename Parent>
    void ReadState::on_complete(Parent& parent)
    {
        INFO("Printti")
        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        INFO("READ TASK IS COMPLETE!");
        auto response = std::make_unique<Response>(Status::OK);
        response->keep_alive = _connection._keep_alive;
        response->body(std::move(_task.buffer()));

        SendState send_state{
            SendResponseTask(std::move(_connection), std::move(response)),
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
