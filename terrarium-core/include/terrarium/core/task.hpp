#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>

#include <concepts>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory_resource>
#include <queue>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace terra::core {
    class TERRA_CORE_API TaskPool {
        using TaskFunction = std::move_only_function<void(std::pmr::memory_resource&)>;

        template<std::invocable<std::pmr::memory_resource&> T>
        using TaskResult = std::invoke_result_t<T, std::pmr::memory_resource&>;

    public:
        explicit TaskPool(usize num_workers);
        ~TaskPool() noexcept;

        TaskPool(TaskPool&&) noexcept = delete;
        TaskPool(const TaskPool&) = delete;

        auto post(std::invocable<std::pmr::memory_resource&> auto&& task) -> void {
            enqueue(std::forward<decltype(task)>(task));
        }

        [[nodiscard]] auto submit(
            std::invocable<std::pmr::memory_resource&> auto&& task
        ) -> std::future<std::invoke_result_t<decltype(task), std::pmr::memory_resource&>> {
            std::packaged_task packaged_task{ std::forward<decltype(task)>(task) };
            auto future = packaged_task.get_future();

            enqueue(
                [task = std::move(packaged_task)](std::pmr::memory_resource& scratch) mutable -> void {
                    std::invoke(task, scratch);
                }
            );

            return future;
        }

    private:
        std::vector<std::jthread> m_workers{};

        Mutex<std::queue<TaskFunction>> m_tasks{};
        std::condition_variable_any m_tasks_notifier{};

        auto worker_loop(std::stop_token&& token) -> void;

        auto enqueue(TaskFunction&& task) -> void;
    };
}
