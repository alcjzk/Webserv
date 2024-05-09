#pragma once

#include <vector>
#include <memory>
#include <map>

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
        static void     handle_child_exit(int);
        static bool     child_exited();
        void            child_add(pid_t pid_to_add, Task& task_to_add);
        void            child_remove(pid_t to_remove);

        void run_impl();

        std::vector<std::unique_ptr<Task>> _tasks;
        std::map<pid_t, Task&>             _pids_alive;
        static bool                        _is_interrupt_signaled;
        static bool                        _is_child_exited;
        static Runtime                     _instance;
};
