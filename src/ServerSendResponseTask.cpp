#include <unistd.h>
#include <cassert>
#include <chrono>
#include "ServerSendResponseTask.hpp"
#include "Log.hpp"

ServerSendResponseTask::ServerSendResponseTask(const Config& config, int fd, Response* response)
    : Task(fd, Writable, std::chrono::system_clock::now() + config.send_timeout()),
      _response(response)
{
}

ServerSendResponseTask::~ServerSendResponseTask()
{
    delete _response;
}

void ServerSendResponseTask::run()
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
    _is_complete = true;
    (void)close(_fd);
}

void ServerSendResponseTask::abort()
{
    INFO("ServerSendResponseTask for fd " << _fd << " timed out.");
    _is_complete = true;
    (void)close(_fd);
}
