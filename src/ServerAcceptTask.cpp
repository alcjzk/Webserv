#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include "ServerAcceptTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "ServerReceiveRequestTask.hpp"

ServerAcceptTask::ServerAcceptTask(const Server& server)
    : Task(server.fd(), Readable), _server(server)
{
}

void ServerAcceptTask::run()
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
            (void)close(fd);
            throw std::runtime_error(strerror(errno));
        }
        INFO("Client connected on fd " << fd);
        Runtime::enqueue(new ServerReceiveRequestTask(_server, fd));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}

ServerAcceptTask::~ServerAcceptTask() {}
