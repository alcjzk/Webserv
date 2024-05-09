#pragma once

#include <chrono>
#include <optional>

/// Interface for a runtime task.
class Task
{
    public:
        using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
        using Seconds = std::chrono::seconds;

        enum class WaitFor
        {
            Readable,
            Writable,
        };

        virtual ~Task() = default;

        bool is_expired_at(TimePoint time_point) const;

        virtual void                   run() = 0;
        virtual void                   abort() = 0;
        virtual void                   terminate(bool error) = 0;
        virtual int                    fd() const = 0;
        virtual WaitFor                wait_for() const = 0;
        virtual std::optional<Seconds> expire_time() const = 0;
        virtual bool                   is_complete() const = 0;
        virtual TimePoint              last_run() const = 0;
        virtual void                   last_run(TimePoint timepoint) = 0;
};
