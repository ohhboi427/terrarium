#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/event.hpp>
#include <terrarium/core/plugin.hpp>
#include <terrarium/core/ecs/extractor.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <atomic>
#include <concepts>
#include <functional>
#include <utility>

namespace terra::core {
    struct TERRA_CORE_API AppQuitEvent : IEvent {};

    class TERRA_CORE_API App {
        template<Event E, Extractor... Es>
            requires std2::is_clean_type_v<E>
        using Listener = bool(*)(const E&, Es...);

    public:
        auto run() -> void;

        auto add_plugin(Plugin auto&& plugin) -> App& {
            std::invoke(plugin, *this);

            return *this;
        }

        template<Extractor... Es>
        auto add_system(const System<Es...> system, std::convertible_to<SystemOrdering> auto&&... orderings) -> App& {
            m_schedule.add_system(system, std::forward<decltype(orderings)>(orderings)...);

            return *this;
        }

        template<Event E, Extractor... Es>
            requires std2::is_clean_type_v<E>
        auto register_listener(const Listener<E, Es...> listener) -> App& {
            m_event_bus.register_listener<E>(
                [this, listener](const E& event) -> bool {
                    return std::invoke(listener, event, IExtractor<Es>::operator()(m_world)...);
                }
            );

            return *this;
        }

    private:
        std::atomic<bool> m_running = true;

        EventBus m_event_bus{};

        World m_world{};
        Schedule m_schedule{};
    };
}
