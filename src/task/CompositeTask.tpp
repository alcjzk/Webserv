#pragma once

#include <utility>
#include <type_traits>
#include <optional>
#include <variant>
#include <stdexcept>

#include "Task.hpp"
#include "CompositeTask.hpp"

template <typename... States>
void CompositeTask<States...>::set_complete()
{
    _is_complete = true;
}

template <typename... States> template <typename T> void CompositeTask<States...>::state(
    T&& state
    )
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
        [](auto&& state)
        {
            using T = std::decay_t<decltype(state)>;

            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                state._task.abort();
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
std::optional<Task::TimePoint> CompositeTask<States...>::expire_time() const
{
    return *std::visit(
        [](auto&& state) -> std::template optional<std::template optional<TimePoint>> {
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
        }, _state
    );
}

