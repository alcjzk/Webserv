#include "Task.hpp"

bool Task::operator==(int fd)
{
    return this->fd() == fd;
}

bool Task::is_expired_at(TimePoint time_point) const
{
    if (this->expire_time() && *(this->expire_time()) <= time_point)
        return true;
    return false;
}
