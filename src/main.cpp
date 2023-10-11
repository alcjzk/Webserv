#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include "Config.hpp"

int main()
{
    Config  config;

    int                     status;
    struct addrinfo         hints;
    struct addrinfo         *result;
    int                     socket_fd;
    struct sockaddr_storage client_addr;
    socklen_t               addr_size;
    char                    buffer[512];
    const char              *port;

    hints = (struct addrinfo){};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    port = config.ports().front().c_str();
    status = getaddrinfo(NULL, port, &hints, &result);
    socket_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(socket_fd, result->ai_addr, result->ai_addrlen);
    listen(socket_fd, config.backlog());
    addr_size = sizeof(client_addr);

    while (true)
    {
        int connection_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
        while (true)
        {
            ssize_t bytes_read = recv(connection_fd, buffer, 512, 0);
            if (!bytes_read)
                break ;
            write(STDOUT_FILENO, buffer, bytes_read);
            send(connection_fd, "a", 2, 0);
        }
    }


    return (EXIT_SUCCESS);
}
