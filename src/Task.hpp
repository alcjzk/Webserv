#ifndef TASK_H
#define TASK_H

#include <chrono>
#include <optional>

class Runtime;

class Task
{
    public:
        typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;
        typedef enum WaitFor
        {
            Readable,
            Writable
        } WaitFor;

        virtual ~Task() = default;

        int          fd() const;
        bool         is_complete() const;
        bool         is_expired_at(TimePoint time_point) const;
        WaitFor      wait_for() const;

        bool         operator==(int fd);

        virtual void abort();
        virtual void run() = 0;

    protected:
        Task(int fd, WaitFor wait_for,
            std::optional<TimePoint> expire_time = std::nullopt);

        int                         _fd;
        WaitFor                     _wait_for;
        bool                        _is_complete;
        std::optional<TimePoint>    _expire_time;
};

#endif
