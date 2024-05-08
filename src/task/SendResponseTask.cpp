#include <optional>
#include <unistd.h>
#include <utility>
#include <memory>
#include <stdexcept>
#include "SendResponseTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "File.hpp"
#include "Response.hpp"
#include "BasicTask.hpp"
#include "Task.hpp"
#include "Connection.hpp"
#include "ReceiveRequestTask.hpp"

using std::optional;
using std::unique_ptr;
using Seconds = Task::Seconds;

SendResponseTask::SendResponseTask(Connection&& connection, unique_ptr<Response>&& response)
    : BasicTask(File(), WaitFor::Writable), _connection(std::move(connection)),
      _response(std::move(response)), _expire_time(_connection.config().send_timeout())
{
}

void SendResponseTask::run()
{
    try
    {
        if (!_response->send(_connection.client()))
            return;
        if (_response->keep_alive)
            Runtime::enqueue(new ReceiveRequestTask(std::move(_connection)));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
    _is_complete = true;
}

void SendResponseTask::abort()
{
    INFO("ServerSendResponseTask for fd " << _fd << " timed out.");
    _is_complete = true;
}

int SendResponseTask::fd() const
{
    return _connection.client();
}

optional<Seconds> SendResponseTask::expire_time() const
{
    return _expire_time;
}
