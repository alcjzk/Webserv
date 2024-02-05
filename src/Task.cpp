#include <unistd.h>
#include "Task.hpp"
#include "Runtime.hpp"

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

bool Task::is_expired() const
{
    if (_expire_time && *_expire_time >= std::chrono::system_clock::now())
        return true;
    return false;
}


Task::WaitFor Task::wait_for() const
{
    return _wait_for;
}
