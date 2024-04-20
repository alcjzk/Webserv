#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <stdexcept>
#include <utility>
#include <string.h>
#include "File.hpp"
#include "AcceptTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "BasicTask.hpp"
#include "Server.hpp"
#include "Task.hpp"
#include "Connection.hpp"
#include "ReceiveRequestTask.hpp"

using WaitFor = Task::WaitFor;

AcceptTask::AcceptTask(const Server& server)
    : BasicTask(server.fd(), WaitFor::Readable), _server(server)
{
}

void AcceptTask::run()
{
    int fd;

    try
    {
        fd = accept(_server.fd(), NULL, NULL);
        if (fd == -1)
        {
            throw std::runtime_error(strerror(errno));
        }
        if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        {
            // TODO: This should probably be handled as a warning
            throw std::runtime_error(strerror(errno));
        }
        INFO("Client connected on fd " << fd);
        Connection connection(fd, _server);
        Runtime::enqueue(new ReceiveRequestTask(std::move(connection)));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}
