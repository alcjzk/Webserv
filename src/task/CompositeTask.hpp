#pragma once

#include <variant>
#include <optional>
#include "Task.hpp"

/// CompositeTask for combining multiple tasks into one.
///
/// Refer to ErrorResponseTask for a basic example implementation.
template <typename... States>
class CompositeTask : public Task
{
    public:
        using State = std::variant<std::monostate, States...>;

        /// Marks the task as completed.
        void set_complete();

        /// Replaces the tasks internal state with the provided value.
        template <typename T>
        void state(T&& state);

        virtual void                     run() override;
        virtual void                     abort() override;
        virtual int                      fd() const override;
        virtual WaitFor                  wait_for() const override;
        virtual bool                     is_complete() const override;
        virtual std::optional<TimePoint> expire_time() const override;

    private:
        State _state;
        bool  _is_complete = false;
};

#include "CompositeTask.tpp"
