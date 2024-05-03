#pragma once

#include <vector>
#include <memory>

class Task;

class Runtime
{
    public:
        ~Runtime() = default;
        Runtime(const Runtime&) = delete;
        Runtime(Runtime&&) = delete;

        Runtime& operator=(const Runtime&) = delete;
        Runtime& operator=(Runtime&&) = delete;

        static void enqueue(Task* task);
        static void run();

    private:
        static const int POLL_TIMEOUT_MILLIS = 1000;

        Runtime() = default;

        static Runtime& instance();
        static void     handle_interrupt(int);

        void run_impl();

        std::vector<std::unique_ptr<Task>> _tasks;
        static bool                        _is_interrupt_signaled;
        static Runtime                     _instance;
};
