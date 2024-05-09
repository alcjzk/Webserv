#pragma once

#include <optional>
#include "File.hpp"
#include "Task.hpp"

/// BasicTask provides default properties and overrides for a Task.
class BasicTask : public Task
{
    public:
        virtual void                   abort() override;
        virtual void                   terminate(bool is_error) override;
        virtual int                    fd() const override;
        virtual WaitFor                wait_for() const override;
        virtual bool                   is_complete() const override;
        virtual std::optional<Seconds> expire_time() const override;
        virtual TimePoint              last_run() const override;
        virtual void                   last_run(TimePoint last_run) override;

    protected:
        File      _fd;
        WaitFor   _wait_for;
        bool      _is_complete = false;
        TimePoint _last_run = std::chrono::system_clock::now();

        BasicTask(File&& fd, WaitFor wait_for);
};
