#ifndef RUNTIME_H
# define RUNTIME_H

#include <poll.h>
#include <vector>

class Task;

class Runtime
{
    public:
        ~Runtime();
        static void enqueue(Task* task);
        void run();
        static Runtime& instance();

    private:
        Runtime();
        Runtime(const Runtime&);
        void operator=(const Runtime&);

        static void                     _handle_interrupt(int);
        void                            _dequeue(Task* task);
        Task*                           _task(int fd);

        std::vector<Task*>              _tasks;
        static bool                     _is_interrupt_signaled;
        static Runtime                  _instance;
};

#endif
