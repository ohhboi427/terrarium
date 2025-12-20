#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <concepts>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace terra::core {
    template<Extractor... Es>
    using System = void(*)(Es...);

    namespace detail {
        using SystemHandle = void(*)();
    }

    struct TERRA_CORE_API Before {
        detail::SystemHandle system;

        template<Extractor... Es>
        explicit Before(const System<Es...> system) noexcept
            : system{ reinterpret_cast<detail::SystemHandle>(system) } {}
    };

    struct TERRA_CORE_API After {
        detail::SystemHandle system;

        template<Extractor... Es>
        explicit After(const System<Es...> system) noexcept
            : system{ reinterpret_cast<detail::SystemHandle>(system) } {}
    };

    using SystemOrdering = std::variant<Before, After>;

    class TERRA_CORE_API Schedule {
        using SystemFunction = std::move_only_function<void(World&)>;

        struct SystemMetadata {
            SystemFunction function;
            std::vector<SystemOrdering> orderings;
        };

    public:
        Schedule() = default;
        Schedule(Schedule&&) noexcept = delete;
        Schedule(const Schedule&) = delete;

        template<Extractor... Es>
        auto add_system(const System<Es...> system, std::convertible_to<SystemOrdering> auto&&... orderings) -> void {
            m_systems_metadata.try_emplace(
                reinterpret_cast<detail::SystemHandle>(system),
                SystemMetadata{
                    .function = [system](World& world) noexcept -> void {
                        system(IExtractor<Es>::operator()(world)...);
                    },
                    .orderings = { std::forward<decltype(orderings)>(orderings)... }
                }
            );
        }

        auto build() -> void;
        auto run(World& world) -> void;

    private:
        std::vector<SystemFunction> m_systems{};
        std::unordered_map<detail::SystemHandle, SystemMetadata> m_systems_metadata{};
    };
}
