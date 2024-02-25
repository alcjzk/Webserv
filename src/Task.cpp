#include <unistd.h>
#include "Task.hpp"
#include "Log.hpp"

Task::Task(int fd, WaitFor wait_for, std::optional<TimePoint> expire_time)
    : _fd(fd), _wait_for(wait_for), _is_complete(false),
    _expire_time(expire_time) {}

bool Task::operator==(int fd)
{
    return _fd == fd;
}

int Task::fd() const
{
    return _fd;
}

bool Task::is_complete() const
{
    return _is_complete;
}

bool Task::is_expired_at(TimePoint time_point) const
{
    if (_expire_time && *_expire_time <= time_point)
        return true;
    return false;
}

Task::WaitFor Task::wait_for() const
{
    return _wait_for;
}

void Task::abort()
{
    INFO("Task for fd " << _fd << " timed out");
    _is_complete = true;
    (void)close(_fd);
}
