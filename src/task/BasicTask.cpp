#include <utility>
#include <optional>
#include "File.hpp"
#include "Log.hpp"
#include "Task.hpp"
#include "BasicTask.hpp"

using Seconds = Task::Seconds;
using TimePoint = Task::TimePoint;

void BasicTask::abort()
{
    WARN(
        "BasicTask for fd `" << _fd << "`"
                             << "was aborted"
    );
    _is_complete = true;
}

int BasicTask::fd() const
{
    return _fd;
}

Task::WaitFor BasicTask::wait_for() const
{
    return _wait_for;
}

bool BasicTask::is_complete() const
{
    return _is_complete;
}

std::optional<Seconds> BasicTask::expire_time() const
{
    return std::nullopt;
}

TimePoint BasicTask::last_run() const
{
    return _last_run;
}

void BasicTask::last_run(TimePoint last_run)
{
    _last_run = last_run;
}

void BasicTask::terminate(bool is_error)
{
    INFO(
        "BasicTask for fd `" << _fd << "`"
                             << "was terminated"
    );
    _is_complete = true;
}

BasicTask::BasicTask(File&& fd, WaitFor wait_for) : _fd(std::move(fd)), _wait_for(wait_for) {}
