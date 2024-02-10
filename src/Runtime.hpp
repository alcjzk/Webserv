#pragma once

#include <vector>

class Task;

class Runtime
{
    public:
        ~Runtime();
        Runtime(const Runtime&) = delete;
        Runtime(Runtime&&) = delete;

        Runtime&        operator=(const Runtime&) = delete;
        Runtime&        operator=(Runtime&&) = delete;

        static void     enqueue(Task* task);
        static void     run();

    private:
        static const int    POLL_TIMEOUT_MILLIS = 1000;

        Runtime() = default;

        static Runtime&    instance();
        static void        handle_interrupt(int);

        Task*              task(int fd);

        std::vector<Task*> _tasks;
        static bool        _is_interrupt_signaled;
        static Runtime     _instance;
};
