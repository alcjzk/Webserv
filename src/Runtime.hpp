#pragma once

#include <poll.h>
#include <vector>

class Task;

class Runtime
{
    public:
        ~Runtime();
        Runtime(const Runtime&) = delete;
        Runtime(const Runtime&&) = delete;

        Runtime& operator=(const Runtime&) = delete;
        Runtime& operator=(const Runtime&&) = delete;

        static void enqueue(Task* task);
        void run();
        static Runtime& instance();

    private:
        Runtime() = default;

        static void                     _handle_interrupt(int);
        void                            _dequeue(Task* task);
        Task*                           _task(int fd);

        std::vector<Task*>              _tasks;
        static bool                     _is_interrupt_signaled;
        static Runtime                  _instance;
};
