#pragma once

#include <optional>
#include <sys/types.h>

/// RAII wrapper for a child process.
class Child
{
    public:
        ~Child();
        Child(pid_t);
        Child(Child&& other);
        Child(const Child&) = delete;

        Child& operator=(const Child&) = delete;
        Child& operator=(Child&& other);

        int wait();

    private:
        std::optional<pid_t> _pid;
};
