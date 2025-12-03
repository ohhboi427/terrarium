#include <terrarium/core/task.hpp>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace terra::core {
    namespace {
        std::vector<std::jthread> g_workers{};

        std::mutex g_tasks_mutex{};
        std::condition_variable_any g_tasks_notifier{};
        std::queue<std::move_only_function<void()>> g_tasks{};

        auto thread_loop(const std::stop_token& token) -> void {
            while(true) {
                std::move_only_function<void()> task{};

                {
                    std::unique_lock lock{ g_tasks_mutex };
                    g_tasks_notifier.wait(
                        lock,
                        token,
                        [] noexcept -> bool {
                            return !g_tasks.empty();
                        }
                    );

                    if(token.stop_requested() && g_tasks.empty()) {
                        return;
                    }

                    task = std::move(g_tasks.front());
                    g_tasks.pop();
                }

                task();
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

    auto detail::enqueue_task(std::move_only_function<void()> task) -> void {
        {
            std::unique_lock lock{ g_tasks_mutex };
            g_tasks.push(std::move(task));
        }

        g_tasks_notifier.notify_one();
    }
}
