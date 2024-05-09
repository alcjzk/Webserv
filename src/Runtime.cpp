#include <algorithm>
#include <memory>
#include <system_error>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <chrono>
#include <sys/wait.h>
#include "Log.hpp"
#include "Runtime.hpp"
#include "Task.hpp"
#include "Log.hpp"

using WaitFor = Task::WaitFor;

using std::vector;

bool Runtime::_is_interrupt_signaled = false;

bool Runtime::_is_child_exited = false;

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
        INFO("Poll loop start")
        try
        {
            vector<struct pollfd> pollfds;
            pollfds.reserve(_tasks.size());

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
            for (size_t idx = 0; idx < pollfds.size(); idx++)
            {
                const auto& pollfd = pollfds[idx];

                if (pollfd.revents)
                {
                    _tasks[idx]->run();
                    _tasks[idx]->last_run(now);
                }
                else if (auto expire_time = _tasks[idx]->expire_time())
                {
                    if (now - _tasks[idx]->last_run() >= *expire_time)
                        _tasks[idx]->abort();
                }
            }

            _tasks.erase(
                std::remove_if(
                    _tasks.begin(), _tasks.end(),
                    [](const auto& task) { return task->is_complete(); }
                ),
                _tasks.end()
            );
        }
        catch (const std::exception& error)
        {
            ERR("Runtime: " << error.what());
        }
    }
}

void Runtime::handle_interrupt(int)
{
    _is_interrupt_signaled = true;
}
