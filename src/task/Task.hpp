#ifndef TASK_H
#define TASK_H

#include <chrono>
#include <optional>
#include "File.hpp"

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

        Task(Task&&) noexcept = default;

        int     fd() const;
        bool    is_complete() const;
        bool    is_expired_at(TimePoint time_point) const;
        WaitFor wait_for() const;

        bool operator==(int fd);

        virtual void abort();
        virtual void run() = 0;

        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;
        Task& operator=(Task&&) = delete;

    protected:
        explicit Task(
            int fd, WaitFor wait_for, std::optional<TimePoint> expire_time = std::nullopt
        );
        Task(File&& file, WaitFor wait_for, std::optional<TimePoint> expire_time = std::nullopt);

        File                     _fd;
        WaitFor                  _wait_for;
        bool                     _is_complete;
        std::optional<TimePoint> _expire_time;
};

#endif
