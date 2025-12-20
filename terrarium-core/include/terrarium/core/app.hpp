#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace terra::core {
    class App;

    template<typename T>
    struct is_plugin : std::is_invocable<T, App&> {};

    template<typename T>
    constexpr bool is_plugin_v = is_plugin<T>::value;

    template<typename T>
    concept Plugin = is_plugin_v<T>;

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
