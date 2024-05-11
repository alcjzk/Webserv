#include "Child.hpp"
#include <sys/wait.h>
#include <signal.h>
#include <utility>

Child::~Child()
{
    if (_pid)
    {
        int status = 0;
        kill(_pid.value(), SIGKILL);
        (void)waitpid(_pid.value(), &status, 0);
        _pid.reset();
    }
}

Child::Child(pid_t pid) : _pid(pid) {}

Child::Child(Child&& other) : _pid(std::exchange(other._pid, std::nullopt)) {}

Child& Child::operator=(Child&& other)
{
    if (this == &other)
        return *this;
    _pid = std::exchange(other._pid, std::nullopt);
    return *this;
}

int Child::wait()
{
    int status = 0;
    (void)waitpid(_pid.value(), &status, 0);
    _pid.reset();
    return WEXITSTATUS(status);
}
