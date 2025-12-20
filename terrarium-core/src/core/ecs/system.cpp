#include <terrarium/core/ecs/system.hpp>

#include <ranges>
#include <utility>

namespace terra::core {
    auto Schedule::build() -> void {
        for(auto& [function, orderings] : m_systems_metadata | std::views::values) {
            m_systems.emplace_back(std::move(function));
        }
    }

    auto Schedule::run(World& world) -> void {
        for(auto& system : m_systems) {
            system(world);
        }
    }
}
