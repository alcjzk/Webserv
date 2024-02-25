#include <unistd.h>
#include <cassert>
#include "ServerSendResponseTask.hpp"
#include "Log.hpp"

ServerSendResponseTask::ServerSendResponseTask(const Config& config, int fd, Response* response)
    : Task(fd, Writable), _response(response)
{
    if (config.send_timeout())
        _expire_time = std::chrono::system_clock::now() + config.send_timeout().value();
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
