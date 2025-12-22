#include <terrarium/core/task.hpp>

#include <memory>
#include <memory_resource>
#include <stop_token>
#include <utility>

namespace terra::core {
    TaskPool::TaskPool(const usize num_workers) {
        m_workers.reserve(num_workers);
        for(usize i = 0U; i < num_workers; ++i) {
            m_workers.emplace_back(
                [this](std::stop_token token) -> void {
                    worker_loop(std::move(token));
                }
            );
        }
    }

    TaskPool::~TaskPool() noexcept {
        m_workers.clear();
    }

    auto TaskPool::worker_loop(std::stop_token&& token) -> void {
        static constexpr auto SCRATCH_SIZE = 4_mb;

        const auto scratch = std::make_unique_for_overwrite<byte[]>(SCRATCH_SIZE);
        std::pmr::monotonic_buffer_resource resource{
            scratch.get(),
            SCRATCH_SIZE,
            std::pmr::new_delete_resource()
        };

        while(true) {
            Task task{};

            {
                auto tasks = m_tasks.lock();
                m_tasks_notifier.wait(
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

    auto TaskPool::enqueue(Task&& task) -> void {
        {
            auto tasks = m_tasks.lock();
            tasks->push(std::move(task));
        }

        m_tasks_notifier.notify_one();
    }
}
