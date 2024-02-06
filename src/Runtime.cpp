#include <algorithm>
#include <cassert>
#include <system_error>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <chrono>
#include "Log.hpp"
#include "Runtime.hpp"
#include "Task.hpp"

using std::vector;

bool Runtime::_is_interrupt_signaled = false;

Runtime::~Runtime()
{
    for (auto task : _tasks)
    {
        delete task;
    }
}

Runtime& Runtime::instance()
{
    static Runtime runtime;

    return runtime;
}

void Runtime::enqueue(Task* task)
{
    instance()._tasks.push_back(task);
}

void Runtime::dequeue(Task* task)
{
    vector<Task*>::iterator it = std::find(_tasks.begin(), _tasks.end(), task);
    _tasks.erase(it);
    delete task;
}

void Runtime::run()
{
    if (signal(SIGINT, handle_interrupt) == SIG_ERR)
        throw std::system_error(errno, std::system_category());

    while (!_is_interrupt_signaled)
    {
        vector<struct pollfd>   pollfds;

        for (auto task : instance()._tasks)
        {
            short events = 0;
            if (task->wait_for() == Task::Readable)
                events = POLLIN;
            else if (task->wait_for() == Task::Writable)
                events = POLLOUT;
            pollfds.push_back({task->fd(), events, 0});
        }

        if (poll(pollfds.data(), pollfds.size(), POLL_TIMEOUT_MILLIS) == -1)
        {
            // Don't throw when poll errors due to signal
            if (errno != EINTR)
                throw std::system_error(errno, std::system_category());
        }

        auto head = pollfds.cbegin();
        for (auto task : instance()._tasks)
        {
            auto pollfd = std::find_if(head, pollfds.cend(), [task](const auto& pollfd){
                if (task->fd() == pollfd.fd)
                    return true;
                return false;
            });
            head = pollfd + 1;
            if (pollfd->revents)
            {
                Task* task = instance().task(pollfd->fd);
                assert(task);
                task->run();
                if (task->is_complete())
                    instance().dequeue(task);
            }
            else if (task->is_expired_at(std::chrono::system_clock::now()))
            {
                INFO("Task for fd " << task->fd() << " timed out.");
                instance().dequeue(task);
            }
        }
    }
}

Task* Runtime::task(int fd)
{
    vector<Task*>::iterator task = std::find_if(
        _tasks.begin(), _tasks.end(), [fd](auto task){
            return task->fd() == fd;
        });
    if (task == _tasks.end())
        return nullptr;
    return *task;
}

void Runtime::handle_interrupt(int)
{
    _is_interrupt_signaled = true;
}
