#pragma once

#include <chrono>
#include <optional>

/// Interface for a runtime task.
class Task
{
    public:
        typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

        enum class WaitFor
        {
            Readable,
            Writable,
        };

        virtual ~Task() = default;

        bool is_expired_at(TimePoint time_point) const;

        /// TODO: Used?
        bool operator==(int fd);

        virtual void                     run() = 0;
        virtual void                     abort() = 0;
        virtual int                      fd() const = 0;
        virtual WaitFor                  wait_for() const = 0;
        virtual bool                     is_complete() const = 0;
        virtual std::optional<TimePoint> expire_time() const = 0;
};
