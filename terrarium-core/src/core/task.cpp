#include <core/task.hpp>
#include <terrarium/core/task.hpp>

#include <terrarium/core/mutex.hpp>

#include <condition_variable>
#include <memory>
#include <memory_resource>
#include <queue>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace terra::core {
    namespace {
        std::vector<std::jthread> g_workers{};

        SharedMutex<std::queue<detail::Task>> g_tasks{};
        std::condition_variable_any g_tasks_notifier{};

        auto thread_loop(const std::stop_token& token) -> void {
            static constexpr auto SCRATCH_SIZE = 4_mb;

            const auto scratch = std::make_unique_for_overwrite<byte[]>(SCRATCH_SIZE);
            std::pmr::monotonic_buffer_resource resource{
                scratch.get(),
                SCRATCH_SIZE,
                std::pmr::new_delete_resource()
            };

            while(true) {
                detail::Task task{};

                {
                    auto tasks = g_tasks.lock();
                    g_tasks_notifier.wait(
                        tasks,
                        token,
                        [&tasks] noexcept -> bool {
                            return !tasks->empty();
                        }
                    );

                    if(token.stop_requested() && tasks->empty()) {
                        return;
                    }

                    task = std::move(tasks->front());
                    tasks->pop();
                }

                task(resource);
                resource.release();
            }
        }
    }

    auto thread_pool_init(const usize num_workers) -> void {
        for(usize i = 0U; i < num_workers; ++i) {
            g_workers.emplace_back(
                [](const std::stop_token& token) -> void {
                    thread_loop(token);
                }
            );
        }
    }

    auto thread_pool_destroy() -> void {
        g_workers.clear();
    }

    auto detail::enqueue_task(Task&& task) -> void {
        {
            auto tasks = g_tasks.lock();
            tasks->push(std::move(task));
        }

        g_tasks_notifier.notify_one();
    }
}
