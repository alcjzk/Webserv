#include <unistd.h>
#include <cassert>
#include <chrono>
#include <utility>
#include <stdexcept>
#include "SendResponseTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "File.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "BasicTask.hpp"
#include "Task.hpp"
#include "ReceiveRequestTask.hpp"

SendResponseTask::SendResponseTask(const Server& server, File&& file, Response* response)
    : BasicTask(
          std::move(file), WaitFor::Writable,
          std::chrono::system_clock::now() + server.config().send_timeout()
      ),
      _response(response), _server(server)
{
}

void SendResponseTask::run()
{
    try
    {
        if (!_response->send(_fd))
            return;
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
    catch (...)
    {
        assert(false);
    }
    if (_response->_connection == Response::Connection::KeepAlive)
    {
        Runtime::enqueue(new ReceiveRequestTask(_server, std::move(_fd)));
    }
    _is_complete = true;
}

void SendResponseTask::abort()
{
    INFO("ServerSendResponseTask for fd " << _fd << " timed out.");
    _is_complete = true;
}
