#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "Server.hpp"

const size_t Server::_buffer_size = 512;
volatile bool Server::_is_interrupt_signaled = false;

Server::Server(const Config &config) : _config(config) {}

Server::~Server()
{
    freeaddrinfo(_address_info);
    cout << "Graceful exit" << endl;
}

void Server::_connect(const char *port)
{
    int             status;
    struct addrinfo hints;
    int             fd;

    hints = (struct addrinfo){};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &_address_info);
    if (status != 0)
        throw gai_strerror(status);

    // TODO: Error handling + fd manual close? ----
    fd = socket(_address_info->ai_family, _address_info->ai_socktype, _address_info->ai_protocol);
    fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    bind(fd, _address_info->ai_addr, _address_info->ai_addrlen);
    listen(fd, _config.backlog());
    // -----

    cout << "listening on socket " << fd << endl;
    _pollfds.push_back((struct pollfd) {
        .fd = fd,
        .events = POLLIN,
        .revents = 0,
    });
    _tasks.push_back((_Task){
        .fd = fd,
        .func = &Server::_accept_connection
    });
}

void Server::start()
{
    vector<_Task>::iterator  task;

    if (signal(SIGINT, _handle_interrupt) == SIG_ERR)
        throw "Failed to register handler for SIGINT";
    this->_connect(_config.ports().front().c_str());
    while (!_is_interrupt_signaled)
    {
        poll(_pollfds.data(), _pollfds.size(), 0);
        for (
            vector<struct pollfd>::size_type i = 0, size = _pollfds.size();
            i < size;
            i++)
        {
            if (_pollfds[i].revents)
            {
                 // TODO Necessary or not? a.e. does poll() overwrite .revents
                 // for a pollfd structure that has no pending events?
                _pollfds[i].revents = 0;
                vector<_Task>::iterator task = _task(_pollfds[i].fd);
                (this->*task->func)(_pollfds[i].fd);
            }
        }
    }
}

void Server::_handle_request(int fd)
{
    vector<struct pollfd>::iterator pollfd;
    vector<_Task>::iterator          task;
    ssize_t                         bytes_read;
    char                            buffer[_buffer_size];

    bytes_read = recv(fd, buffer, _buffer_size, 0);
    if (bytes_read == -1)
        throw "recv error";
    // TODO: This needs to be handled gracefully
    if (bytes_read == 0)
        throw "client unexpectedly closed the connection";
    buffer[bytes_read] = '\0';
    task = _task(fd);
    task->func = &Server::_send_response;
    pollfd = _pollfd(fd);
    pollfd->events = POLLOUT;
}

void Server::_accept_connection(int fd)
{
    int connection_fd;

    connection_fd = accept(fd, NULL, NULL);
    if (connection_fd == -1)
        throw "Failed to accept connection from socket";
    cout << "connected on " << connection_fd << endl;
    (void)fcntl(connection_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    _pollfds.push_back((struct pollfd) {
        .fd = connection_fd,
        .events = POLLIN,
        .revents = 0
    });
    _tasks.push_back((_Task) {
        .fd = connection_fd,
        .func = &Server::_handle_request
    });
}

void    Server::_send_response(int fd)
{
    vector<struct pollfd>::iterator pollfd;
    vector<_Task>::iterator          task;

    // TODO: Error handling for send
    send(fd, "Msg received", 12, 0);
    task = _task(fd);
    _tasks.erase(task);
    pollfd = _pollfd(fd);
    _pollfds.erase(pollfd);
    close(fd);
}

vector<Server::_Task>::iterator Server::_task(int fd)
{
    vector<_Task>::iterator it = _tasks.begin();
    while (it != _tasks.end())
    {
        if (it->fd == fd)
            return it;
        it++;
    }
    throw "Unknown task";
}

vector<struct pollfd>::iterator Server::_pollfd(int fd)
{
    vector<struct pollfd>::iterator it = _pollfds.begin();
    while (it != _pollfds.end())
    {
        if (it->fd == fd)
            return it;
        it++;
    }
    throw "Unknown pollfd";
}

void Server::_handle_interrupt(int)
{
    Server::_is_interrupt_signaled = true;
}
