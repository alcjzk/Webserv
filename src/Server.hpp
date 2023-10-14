#ifndef SERVER_H
# define SERVER_H

#include <vector>
#include <poll.h>
#include <netdb.h>
#include "Config.hpp"
#include "Response.hpp"

using namespace std;

class Server {
    public:
        Server(const Config &config);
        ~Server();

        void    start();

    private:
        static const size_t     _buffer_size; // TODO: move to config?

        typedef void (Server::* _TF)(int fd);

        typedef struct _Task
        {
            int fd;
            _TF  func;
        }   _Task;

        const Config            &_config;
        vector<_Task>            _tasks;
        vector<struct pollfd>   _pollfds;
        vector<Response>        _responses; // TODO: Queued responses
        struct addrinfo         *_address_info;

        void                            _connect(const char *port);
        void                            _handle_request(int fd);
        void                            _accept_connection(int fd);
        void                            _send_response(int fd);

        vector<_Task>::iterator         _task(int fd);
        vector<struct pollfd>::iterator _pollfd(int fd);

        // TODO: Some type of singleton impl might be a good option to
        // safeguard these
        static volatile bool            _is_interrupt_signaled;
        static void                     _handle_interrupt(int);
};

#endif
