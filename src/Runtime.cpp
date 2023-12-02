#include <algorithm>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "Runtime.hpp"
#include "Task.hpp"

using std::vector;

bool Runtime::_is_interrupt_signaled = false;

Runtime::Runtime() {}

Runtime::~Runtime()
{
    vector<Task *>::iterator task = _tasks.begin();
    for (; task != _tasks.end(); task++)
    {
        delete *task;
    }
    std::system("leaks -q webserv"); // FIXME: Make conditional
}

Runtime &Runtime::instance()
{
    static Runtime runtime;

    return runtime;
}

void Runtime::enqueue(Task *task)
{
    instance()._tasks.push_back(task);
}

void Runtime::_dequeue(Task *task)
{
    vector<Task *>::iterator it = std::find(_tasks.begin(), _tasks.end(), task);
    _tasks.erase(it);
    delete task;
}

void Runtime::run()
{
    extern int errno;

    if (signal(SIGINT, _handle_interrupt) == SIG_ERR)
        throw "Failed to register handler for SIGINT";
    while (!_is_interrupt_signaled)
    {
        vector<struct pollfd> pollfds;

        vector<Task *>::iterator task = _tasks.begin();
        for (; task != _tasks.end(); task++)
        {
            short events = 0;
            if ((*task)->wait_for() == Task::Readable)
                events = POLLIN;
            else if ((*task)->wait_for() == Task::Writable)
                events = POLLOUT;
            pollfds.push_back((struct pollfd){
                .fd = (*task)->fd(),
                .events = events,
                .revents = 0});
        }

        // Don't throw when poll errors due to signal
        if (poll(pollfds.data(), pollfds.size(), -1) == -1 && errno != EINTR)
            throw strerror(errno);

        vector<struct pollfd>::iterator pollfd = pollfds.begin();
        for (; pollfd != pollfds.end(); pollfd++)
        {
            if (pollfd->revents)
            {
                Task *task = _task(pollfd->fd);
                task->run();
                if (task->is_complete())
                    _dequeue(task);
            }
        }
    }
}

Task *Runtime::_task(int fd)
{
    vector<Task *>::iterator task = _tasks.begin();
    for (; task != _tasks.end(); task++)
    {
        if (**task == fd)
        {
            return (*task);
        }
    }
    throw "Unknown task";
}

void Runtime::_handle_interrupt(int)
{
    _is_interrupt_signaled = true;
}
