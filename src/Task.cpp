#include <unistd.h>
#include "Task.hpp"
#include "Runtime.hpp"

Task::Task(int fd, WaitFor wait_for) : _fd(fd), _wait_for(wait_for), _is_complete(false) {}

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

Task::WaitFor Task::wait_for() const
{
    return _wait_for;
}