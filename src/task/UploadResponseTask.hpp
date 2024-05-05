#pragma once

#include "CompositeTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Request.hpp"
#include "Path.hpp"
#include "WriteTask.hpp"
#include "SendResponseTask.hpp"
#include "Connection.hpp"

namespace upload_response_task
{
    struct UploadState
    {
        public:
            WriteTask   _task;
            Connection  _connection;
            std::string _location;

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

    class UploadResponseTask : public CompositeTask<UploadState, SendState, ErrorState>
    {
        public:
            UploadResponseTask(
                Connection&& connection, Request& request, const Path& uploads_path, const Path& uri
            );
    };

    // Template impls

    template <typename Parent>
    void UploadState::on_complete(Parent& parent)
    {
        if (_task.is_error())
        {
            Status status = Status::INTERNAL_SERVER_ERROR;

            ErrorState error_state{
                ErrorResponseTask(std::move(_connection), status),
            };
            return parent.state(std::move(error_state));
        }

        auto response = std::make_unique<Response>(Status::CREATED);
        response->keep_alive = _connection._keep_alive;
        response->headers().insert_or_assign({FieldName::LOCATION, _location});
        response->body(R"(<a href="/uploads">Go to uploads.</a>)");

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

} // namespace upload_response_task

using UploadResponseTask = upload_response_task::UploadResponseTask;
