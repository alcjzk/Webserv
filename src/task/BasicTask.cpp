#include <utility>
#include <optional>
#include "File.hpp"
#include "Log.hpp"
#include "Task.hpp"
#include "BasicTask.hpp"

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

std::optional<Task::TimePoint> BasicTask::expire_time() const
{
    return _expire_time;
}

BasicTask::BasicTask(File&& fd, WaitFor wait_for, std::optional<TimePoint> expire_time)
    : _fd(std::move(fd)), _wait_for(wait_for), _expire_time(expire_time)
{
}
