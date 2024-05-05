#pragma once

#include "CompositeTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Request.hpp"
#include "Path.hpp"
#include "WriteTask.hpp"
#include "SendResponseTask.hpp"
#include "Connection.hpp"
#include "CGIReadTask.hpp"
#include "CGIWriteTask.hpp"
#include <ctime>
#include <fcntl.h>

namespace cgi_creation_task
{
    // struct StartState
    // {
    //     public:
    //         pid_t        _pid;
    //         Connection   _connection;
    //         std::string  _location;

    //         void SetEnv(const std::string& key, const std::string& value);
    //         void QueryString(const std::string& query_string);
    //         void SignalhandlerChild(int sig);
    //         char** Environment();
    //         template <typename Parent>
    //         void on_complete(Parent& parent);
    // };

    struct ReadState
    {
        public:
            CGIReadTask       _task;
            std::vector<char> _response_body;
            Connection  _connection;

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

    class CGICreationTask : public CompositeTask<ReadState, SendState, ErrorState>
    {
        public:
            CGICreationTask(
                Connection&& connection, Request& request, const Path& uri, Config& config
            );

        private:
            std::vector<char*> _environment;
            pid_t              _peed;
    };

    // Template impls

    // template <typename Parent>
    // void StartState::on_complete(Parent& parent)
    // {
    //     if (_task.is_error())
    //     {
    //         Status status = Status::INTERNAL_SERVER_ERROR;

    //         ErrorState error_state{
    //             ErrorResponseTask(std::move(_connection), status),
    //         };
    //         return parent.state(std::move(error_state));
    //     }

    //     // Response* response = new Response(Status::CREATED);
    //     // response->_keep_alive = _connection._keep_alive;
    //     // response->header({FieldName::LOCATION, _location});
    //     // response->body(R"(<a href="/uploads">Go to uploads.</a>)");

    //     // SendState send_state{
    //     //     SendResponseTask(std::move(_connection), response),
    //     // };
    //     ReadState read_state {
    //         CGIReadTask(std::move(_pid))
    // };
    //     parent.state(std::move(send_state));
    // }

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

        ReadState read_state {
            CGIReadTask(request, )
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

        Response* response = new Response(Status::OK);
        response->_keep_alive = _connection._keep_alive;
        response->body(_response_body);

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
