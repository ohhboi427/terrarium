#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/ecs/extractor.hpp>

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

    namespace detail {
        template<ConstExtractor... Es>
        using RunCondition = bool(*)(Es...);

        template<ConstExtractor... Es>
        [[nodiscard]] auto wrap_run_condition(const RunCondition<Es...> condition) {
            return [condition](const World& world, const App& app) noexcept -> bool {
                return std::invoke(condition, IExtractor<Es>::operator()(world, app)...);
            };
        }
    }

    template<typename T>
    struct is_run_condition : std::bool_constant<requires(T condition) {
        detail::wrap_run_condition(condition);
    }> {};

    template<typename T>
    constexpr bool is_run_condition_v = is_run_condition<T>::value;

    template<typename T>
    concept RunCondition = is_run_condition_v<T>;

    template<typename T>
    struct is_system_option : std::disjunction<
            is_run_condition<T>,
            std::is_convertible<T, SystemOrdering>
        > {};

    template<typename T>
    constexpr bool is_system_option_v = is_system_option<T>::value;

    template<typename T>
    concept SystemOption = is_system_option_v<T>;

    class TERRA_CORE_API Schedule {
        struct SystemFunction {
            std::move_only_function<void(World&, App&)> function;
            std::move_only_function<bool(const World&, const App&)> condition;
        };

        struct SystemMetadata {
            SystemFunction function;
            std::vector<SystemOrdering> orderings;
        };

    public:
        Schedule() = default;
        Schedule(Schedule&&) noexcept = delete;
        Schedule(const Schedule&) = delete;

        template<Extractor... Es>
        auto add_system(const System<Es...> system, SystemOption auto&&... options) -> void {
            SystemMetadata metadata{
                .function = {
                    .function = [system](World& world, App& app) noexcept -> void {
                        std::invoke(system, IExtractor<Es>::operator()(world, app)...);
                    },
                    .condition = nullptr,
                },
                .orderings = {},
            };

            ([&] {
                if constexpr(is_run_condition_v<decltype(options)>) {
                    metadata.function.condition = detail::wrap_run_condition(options);
                } else if constexpr(std::is_convertible_v<decltype(options), SystemOrdering>) {
                    metadata.orderings.emplace_back(std::forward<decltype(options)>(options));
                }
            }(), ...);

            if(!metadata.function.condition) {
                metadata.function.condition = [](const World&, const App&) noexcept -> bool {
                    return true;
                };
            }

            m_systems_metadata.try_emplace(reinterpret_cast<detail::SystemHandle>(system), std::move(metadata));
        }

        auto build() -> void;
        auto run(World& world, App& app) -> void;

    private:
        std::vector<SystemFunction> m_systems{};
        std::unordered_map<detail::SystemHandle, SystemMetadata> m_systems_metadata{};
    };
}
