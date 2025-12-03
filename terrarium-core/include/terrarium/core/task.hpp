#pragma once

#include <terrarium/defines.hpp>

#include <functional>
#include <future>
#include <type_traits>

namespace terra::core {
    auto thread_pool_init(usize num_workers) -> void;
    auto thread_pool_destroy() -> void;

    namespace detail {
        auto enqueue_task(std::move_only_function<void()> task) -> void;
    }

    template<typename F, typename... Args>
        requires std::is_invocable_r_v<void, F, Args...>
    auto post_task(F&& function, Args&&... args) -> void {
        detail::enqueue_task(
            [function = std::forward<F>(function), ...args = std::forward<Args>(args)] -> void {
                std::invoke(function, args...);
            }
        );
    }

    template<typename F, typename... Args>
        requires std::is_invocable_v<F, Args...>
    auto submit_task(F&& function, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using Ret = std::invoke_result_t<F, Args...>;

        auto task = std::packaged_task{
            [function = std::forward<F>(function), ...args = std::forward<Args>(args)] -> Ret {
                return std::invoke(function, args...);
            }
        };

        auto future = task.get_future();

        detail::enqueue_task(
            [task = std::move(task)] mutable -> void {
                std::invoke(task);
            }
        );

        return future;
    }
}
