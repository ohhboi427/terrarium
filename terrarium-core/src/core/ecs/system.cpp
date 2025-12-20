#include <terrarium/core/ecs/system.hpp>

#include <terrarium/core/debug/assert.hpp>

#include <queue>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>

namespace terra::core {
    auto Schedule::build() -> void {
        std::unordered_map<detail::SystemHandle, std::vector<detail::SystemHandle>> adjacencies{};
        std::unordered_map<detail::SystemHandle, u32> in_degrees{};

        for(const auto handle : m_systems_metadata | std::views::keys) {
            adjacencies.try_emplace(handle);
            in_degrees.try_emplace(handle);
        }

        for(const auto& [handle, metadata] : m_systems_metadata) {
            for(const auto& ordering : metadata.orderings) {
                if(!adjacencies.contains(ordering.reference())) {
                    continue;
                }

                const auto [pre, post] = ordering.to_pair(handle);

                adjacencies[pre].emplace_back(post);
                ++in_degrees[post];
            }
        }

        std::queue<detail::SystemHandle> zero_in_degrees{};
        for(const auto& [handle, in_degree] : in_degrees) {
            if(in_degree == 0U) {
                zero_in_degrees.emplace(handle);
            }
        }

        while(!zero_in_degrees.empty()) {
            const auto current = zero_in_degrees.front();
            zero_in_degrees.pop();

            m_systems.push_back(std::move(m_systems_metadata.at(current).function));

            for(const auto neighbor : adjacencies.at(current)) {
                if(--in_degrees.at(neighbor) == 0U) {
                    zero_in_degrees.emplace(neighbor);
                }
            }
        }

        TERRA_DEBUG_ASSERT(m_systems.size() == m_systems_metadata.size(), "Cyclic dependency detected");
    }

    auto Schedule::run(World& world) -> void {
        for(auto& system : m_systems) {
            system(world);
        }
    }
}
