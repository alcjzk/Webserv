#pragma once

#include "ReadTask.hpp"
#include "SendResponseTask.hpp"
#include "TemplateEngine.hpp"
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
            ReadTask                  _task;
            Connection                _connection;
            std::unique_ptr<Response> _response;

            template <typename Parent>
            void on_complete(Parent& parent);
    };

    class ErrorResponseTask : public CompositeTask<ReadState, SendState>
    {
        public:
            ErrorResponseTask(Connection&& connection, Status status);
            ErrorResponseTask(Connection&& connection, std::unique_ptr<Response>&& response);
    };

    // Template impls

    template <typename Parent>
    void SendState::on_complete(Parent& parent)
    {
        parent.set_complete();
    }

    template <typename Parent>
    void ReadState::on_complete(Parent& parent)
    {
        if (!_task.is_error())
        {
            auto           buffer = std::move(_task).buffer();
            std::string    template_string(buffer.begin(), buffer.end());
            TemplateEngine engine(template_string);

            engine.set_value("status", _response->status().text());
            _response->body(engine.render());
        }

        SendState send_state{SendResponseTask(std::move(_connection), std::move(_response))};
        parent.state(std::move(send_state));
    }
} // namespace error_response_task

using ErrorResponseTask = error_response_task::ErrorResponseTask;
