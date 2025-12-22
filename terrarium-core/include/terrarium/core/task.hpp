#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/ecs/resource.hpp>

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
    class TERRA_CORE_API TaskPool : IResource {
        friend class TaskPoolView;

        using Task = std::move_only_function<void(std::pmr::memory_resource&)>;

        template<std::invocable<std::pmr::memory_resource&> F>
        using TaskResult = std::invoke_result_t<F, std::pmr::memory_resource&>;

    public:
        explicit TaskPool(usize num_workers);
        ~TaskPool() noexcept;

        TaskPool(TaskPool&&) noexcept = delete;
        TaskPool(const TaskPool&) = delete;

        template<std2::invocable_r<void, std::pmr::memory_resource&> F>
        auto post(F&& function) -> void {
            enqueue(std::forward<F>(function));
        }

        template<std::invocable<std::pmr::memory_resource&> F>
        [[nodiscard]] auto submit(F&& function) -> std::future<TaskResult<F>> {
            std::packaged_task task{ std::forward<F>(function) };
            auto future = task.get_future();

            enqueue(
                [task = std::move(task)](std::pmr::memory_resource& scratch) mutable -> void {
                    std::invoke(task, scratch);
                }
            );

            return future;
        }

    private:
        std::vector<std::jthread> m_workers{};

        Mutex<std::queue<Task>> m_tasks{};
        std::condition_variable_any m_tasks_notifier{};

        auto worker_loop(std::stop_token&& token) -> void;

        auto enqueue(Task&& task) -> void;
    };

    class TERRA_CORE_API TaskPoolView : IResource {
    public:
        explicit TaskPoolView(TaskPool& pool) noexcept;

        template<std2::invocable_r<void, std::pmr::memory_resource&> F>
        auto post(F&& function) -> void {
            m_pool.post(std::forward<F>(function));
        }

        template<std::invocable<std::pmr::memory_resource&> F>
        [[nodiscard]] auto submit(F&& function) -> std::future<TaskPool::TaskResult<F>> {
            return m_pool.submit(std::forward<F>(function));
        }

    private:
        TaskPool& m_pool;
    };
}
