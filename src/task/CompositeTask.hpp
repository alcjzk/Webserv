#pragma once

#include <utility>
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

        virtual void                   run() override;
        virtual void                   abort() override;
        virtual int                    fd() const override;
        virtual WaitFor                wait_for() const override;
        virtual bool                   is_complete() const override;
        virtual std::optional<Seconds> expire_time() const override;
        virtual TimePoint              last_run() const override;
        virtual void                   last_run(TimePoint time_point) override;

    private:
        State _state;
        bool  _is_complete = false;
};

template <typename... States>
void CompositeTask<States...>::set_complete()
{
    _is_complete = true;
}

template <typename... States>
template <typename T>
void CompositeTask<States...>::state(T&& state)
{
    _state.template emplace<T>(std::forward<T>(state));
}

template <typename... States>
void CompositeTask<States...>::run()
{
    std::visit(
        [&](auto&& state)
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                state._task.run();
                if (state._task.is_complete())
                    state.on_complete(*this);
            }
            else
            {
                throw std::logic_error("cannot call run() on invariant");
            }
        },
        _state
    );
}

template <typename... States>
void CompositeTask<States...>::abort()
{
    std::visit(
        [&](auto&& state)
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                state._task.abort();
                if (state._task.is_complete())
                    state.on_complete(*this);
            }
            else
            {
                throw std::logic_error("cannot call abort() on invariant");
            }
        },
        _state
    );
}

template <typename... States>
int CompositeTask<States...>::fd() const
{
    return *std::visit(
        [](auto&& state) -> std::template optional<int>
        {
            using T = std::decay_t<decltype(state)>;
            if constexpr (std::is_same_v<T, std::monostate>)
            {
                throw std::logic_error("cannot call fd() on invariant");
                return std::nullopt;
            }
            else
            {
                return state._task.fd();
            }
        },
        _state
    );
}

template <typename... States>
Task::WaitFor CompositeTask<States...>::wait_for() const
{
    return *std::visit(
        [](auto&& state) -> std::template optional<WaitFor>
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (std::is_same_v<T, std::monostate>)
            {
                throw std::logic_error("cannot call wait_for() on invariant");
                return std::nullopt;
            }
            else
            {
                return state._task.wait_for();
            }
        },
        _state
    );
};

template <typename... States>
bool CompositeTask<States...>::is_complete() const
{
    return _is_complete;
}

template <typename... States>
std::optional<Task::Seconds> CompositeTask<States...>::expire_time() const
{
    return *std::visit(
        [](auto&& state) -> std::template optional<std::template optional<Seconds>>
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (std::is_same_v<T, std::monostate>)
            {
                throw std::logic_error("cannot call expire_time() on invariant");
                return std::nullopt;
            }
            else
            {
                return state._task.expire_time();
            }
        },
        _state
    );
}

template <typename... States>
Task::TimePoint CompositeTask<States...>::last_run() const
{
    return *std::visit(
        [](auto&& state) -> std::template optional<TimePoint>
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (std::is_same_v<T, std::monostate>)
            {
                throw std::logic_error("cannot call last_run() on invariant");
                return std::nullopt;
            }
            else
            {
                return state._task.last_run();
            }
        },
        _state
    );
}

template <typename... States>
void CompositeTask<States...>::last_run(Task::TimePoint time_point)
{
    std::visit(
        [=](auto&& state) -> void
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (std::is_same_v<T, std::monostate>)
            {
                throw std::logic_error("cannot call last_run(v) on invariant");
            }
            else
            {
                state._task.last_run(time_point);
            }
        },
        _state
    );
}
