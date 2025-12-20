#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/plugin.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace terra::core {
    class TERRA_CORE_API App {
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

    private:
        World m_world{};
        Schedule m_schedule{};
    };
}
