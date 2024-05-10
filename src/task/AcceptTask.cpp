#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
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
    int             fd;
    std::string     ip;
    struct sockaddr socket_address;
    socklen_t       socket_length = sizeof(socket_address);

    try
    {
        fd = accept(_server.fd(), &socket_address, &socket_length);
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)&socket_address;
        struct in_addr      ip_addr = ipv4->sin_addr;
        std::stringstream   ss;

        ss << std::to_string(int(ip_addr.s_addr & 0xFF)) << "."
           << std::to_string(int((ip_addr.s_addr & 0xFF00) >> 8)) << "."
           << std::to_string(int((ip_addr.s_addr & 0xFF0000) >> 16)) << "."
           << std::to_string(int((ip_addr.s_addr & 0xFF000000) >> 24));

        if (fd == -1)
        {
            throw std::runtime_error(strerror(errno));
        }
        if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        {
            WARN("AcceptTask::run(): fcntl: " << strerror(errno));
        }
        INFO("Client connected on fd " << fd);
        Connection connection(fd, _server, ss.str());
        Runtime::enqueue(new ReceiveRequestTask(std::move(connection)));
    }
    catch (const std::runtime_error& error)
    {
        ERR(error.what());
    }
}
