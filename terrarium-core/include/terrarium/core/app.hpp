#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/event.hpp>
#include <terrarium/core/plugin.hpp>
#include <terrarium/core/task.hpp>
#include <terrarium/core/ecs/extractor.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/tag.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <atomic>
#include <concepts>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace terra::core {
    struct TERRA_CORE_API StartupTag : ITag {};

    struct TERRA_CORE_API UpdateTag : ITag {};

    struct TERRA_CORE_API ShutdownTag : ITag {};

    struct TERRA_CORE_API AppQuitEvent : IEvent {};

    class TERRA_CORE_API App {
        friend class Events;
        friend class Tasks;

    public:
        auto run() -> void;

        auto add_plugin(Plugin auto&& plugin) -> App& {
            std::invoke(plugin, *this);

            return *this;
        }

        template<Tag T, Extractor... Es>
            requires std2::is_clean_type_v<T>
        auto add_system(const System<Es...> system, std::convertible_to<SystemOrdering> auto&&... orderings) -> App& {
            m_schedules[typeid(T)].add_system(system, std::forward<decltype(orderings)>(orderings)...);

            return *this;
        }

        template<Tag T>
        auto run_schedule() -> void {
            const auto it = m_schedules.find(typeid(T));
            if(it == m_schedules.end()) {
                return;
            }

            it->second.run(m_world, *this);
        }

        template<Event E, Extractor... Es>
            requires std2::is_clean_type_v<E>
        auto register_listener(const Listener<E, Es...> listener) -> App& {
            m_event_bus->register_listener<E>(
                [this, listener](const E& event) -> bool {
                    return std::invoke(listener, event, IExtractor<Es>::operator()(m_world, *this)...);
                }
            );

            return *this;
        }

    private:
        std::atomic<bool> m_running = false;

        std::unique_ptr<EventBus> m_event_bus{};
        std::unique_ptr<TaskPool> m_task_pool{};

        World m_world{};
        std::unordered_map<std::type_index, Schedule> m_schedules{};
    };

    class TERRA_CORE_API Events {
    public:
        explicit Events(App& app) noexcept;

        auto dispatch(Event auto&& event) -> void {
            m_bus.dispatch(std::forward<decltype(event)>(event));
        }

    private:
        EventBus& m_bus;
    };

    template<>
    struct TERRA_CORE_API IExtractor<Events> {
        [[nodiscard]] static auto operator()(World&, App& app) noexcept -> Events;
    };

    class TERRA_CORE_API Tasks {
    public:
        explicit Tasks(App& app) noexcept;

        auto post(std::invocable<std::pmr::memory_resource&> auto&& task) -> void {
            m_pool.post(std::forward<decltype(task)>(task));
        }

        [[nodiscard]] auto submit(
            std::invocable<std::pmr::memory_resource&> auto&& task
        ) -> std::future<std::invoke_result_t<decltype(task), std::pmr::memory_resource&>> {
            return m_pool.submit(std::forward<decltype(task)>(task));
        }

    private:
        TaskPool& m_pool;
    };

    template<>
    struct TERRA_CORE_API IExtractor<Tasks> {
        [[nodiscard]] static auto operator()(World&, App& app) noexcept -> Tasks;
    };
}
