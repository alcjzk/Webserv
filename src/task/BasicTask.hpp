#pragma once

#include <optional>
#include "File.hpp"
#include "Task.hpp"

/// BasicTask provides default properties and overrides for a Task.
class BasicTask : public Task
{
    public:
        virtual void                     abort() override;
        virtual int                      fd() const override;
        virtual WaitFor                  wait_for() const override;
        virtual bool                     is_complete() const override;
        virtual std::optional<TimePoint> expire_time() const override;

    protected:
        File                     _fd;
        WaitFor                  _wait_for;
        bool                     _is_complete;
        std::optional<TimePoint> _expire_time;

        BasicTask(File&& fd, WaitFor wait_for, std::optional<TimePoint> expire_time = std::nullopt);
};
