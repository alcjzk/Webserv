#ifndef TASK_H
#define TASK_H

class Runtime;

class Task
{
    public:
        typedef enum WaitFor
        {
            Readable,
            Writable
        } WaitFor;

        virtual ~Task() = default;

        int          fd() const;
        bool         is_complete() const;
        WaitFor      wait_for() const;

        bool         operator==(int fd);

        virtual void run() = 0;

    protected:
        Task(int fd, WaitFor wait_for);

        int     _fd;
        WaitFor _wait_for;
        bool    _is_complete;
};

#endif
