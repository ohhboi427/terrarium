#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/ecs/extractor.hpp>

#include <concepts>
#include <functional>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace terra::core {
    class App;
    class World;

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

    struct TERRA_CORE_API SystemOrdering : std::variant<Before, After> {
        using std::variant<Before, After>::variant;

        [[nodiscard]] constexpr auto reference() const noexcept -> detail::SystemHandle {
            return std::visit(
                Visitor{
                    [](const Before& before) noexcept -> detail::SystemHandle {
                        return before.system;
                    },
                    [](const After& after) noexcept -> detail::SystemHandle {
                        return after.system;
                    },
                },
                *this
            );
        }

        [[nodiscard]] constexpr auto to_pair(
            const detail::SystemHandle other
        ) const noexcept -> std::pair<detail::SystemHandle, detail::SystemHandle> {
            return std::visit(
                Visitor{
                    [other](const Before& before) noexcept -> std::pair<detail::SystemHandle, detail::SystemHandle> {
                        return { other, before.system };
                    },
                    [other](const After& after) noexcept -> std::pair<detail::SystemHandle, detail::SystemHandle> {
                        return { after.system, other };
                    },
                },
                *this
            );
        }
    };

    class TERRA_CORE_API Schedule {
        using SystemFunction = std::move_only_function<void(World&, App&)>;

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
                    .function = [system](World& world, App& app) noexcept -> void {
                        std::invoke(system, IExtractor<Es>::operator()(world, app)...);
                    },
                    .orderings = { std::forward<decltype(orderings)>(orderings)... }
                }
            );
        }

        auto build() -> void;
        auto run(World& world, App& app) -> void;

    private:
        std::vector<SystemFunction> m_systems{};
        std::unordered_map<detail::SystemHandle, SystemMetadata> m_systems_metadata{};
    };
}
