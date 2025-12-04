#pragma once

#include <terrarium/defines.hpp>

#include <functional>
#include <future>
#include <memory_resource>
#include <type_traits>

namespace terra::core {
    auto thread_pool_init(usize num_workers) -> void;
    auto thread_pool_destroy() -> void;

    namespace detail {
        using Task = std::move_only_function<void(std::pmr::memory_resource&)>;

        auto enqueue_task(Task&& task) -> void;
    }

    template<typename F>
        requires std::is_invocable_r_v<void, F, std::pmr::memory_resource&>
    auto post_task(F&& function) -> void {
        detail::enqueue_task(std::forward<F>(function));
    }

    template<typename F>
        requires std::is_invocable_v<F, std::pmr::memory_resource&>
    auto submit_task(F&& function) -> std::future<std::invoke_result_t<F, std::pmr::memory_resource&>> {
        auto task = std::packaged_task{ std::forward<F>(function) };

        auto future = task.get_future();

        detail::enqueue_task(
            [task = std::move(task)](std::pmr::memory_resource& scratch) mutable -> void {
                std::invoke(task, scratch);
            }
        );

        return future;
    }
}
