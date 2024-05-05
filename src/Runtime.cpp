#include <algorithm>
#include <cassert>
#include <memory>
#include <system_error>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <chrono>
#include "Runtime.hpp"
#include "Task.hpp"

using WaitFor = Task::WaitFor;

using std::vector;

bool Runtime::_is_interrupt_signaled = false;

Runtime& Runtime::instance()
{
    static Runtime runtime;

    return runtime;
}

void Runtime::enqueue(Task* task)
{
    instance()._tasks.push_back(std::unique_ptr<Task>(task));
}

void Runtime::run()
{
    instance().run_impl();
}

void Runtime::run_impl()
{
    if (signal(SIGINT, handle_interrupt) == SIG_ERR)
        throw std::system_error(errno, std::system_category());

    while (!_is_interrupt_signaled)
    {
        vector<struct pollfd> pollfds;

        for (const auto& task : _tasks)
        {
            short events = 0;
            if (task->wait_for() == WaitFor::Readable)
                events = POLLIN;
            else if (task->wait_for() == WaitFor::Writable)
                events = POLLOUT;
            pollfds.push_back({task->fd(), events, 0});
        }

        if (poll(pollfds.data(), pollfds.size(), POLL_TIMEOUT_MILLIS) == -1)
        {
            // Don't throw when poll errors due to signal
            if (errno != EINTR)
                throw std::system_error(errno, std::system_category());
        }

        auto now = std::chrono::system_clock::now();

        for (const auto& pollfd : pollfds)
        {
            // NOTE: There's unnecessary iterations here if pollfds / task are in the same order
            auto task = std::find_if(
                _tasks.begin(), _tasks.end(),
                [&pollfd](const auto& task) { return task->fd() == pollfd.fd; }
            );
            assert(task != _tasks.end());

            if (pollfd.revents)
                (*task)->run();
            else if ((*task)->is_expired_at(now))
                (*task)->abort();
        }

        _tasks.erase(
            std::remove_if(
                _tasks.begin(), _tasks.end(), [](const auto& task) { return task->is_complete(); }
            ),
            _tasks.end()
        );
    }
}

void Runtime::handle_interrupt(int)
{
    _is_interrupt_signaled = true;
}
