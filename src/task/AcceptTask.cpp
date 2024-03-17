#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <utility>
#include "AcceptTask.hpp"
#include "Log.hpp"
#include "Runtime.hpp"
#include "ReceiveRequestTask.hpp"

AcceptTask::AcceptTask(const Server& server) : Task(server.fd(), Readable), _server(server) {}

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
        File file(fd);
        if (fcntl(file, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        {
            throw std::runtime_error(strerror(errno));
        }
        INFO("Client connected on fd " << file);
        Runtime::enqueue(new ReceiveRequestTask(_server, std::move(file)));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}
