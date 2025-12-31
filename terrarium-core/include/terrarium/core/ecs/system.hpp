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
    template<Extractor... Es>
    using System = ExtractorFunction<void, Es...>;

    namespace detail {
        struct TERRA_CORE_API SystemHandle {
            using Handle = void(*)();

            Handle handle;

            constexpr SystemHandle() = default;

            template<Extractor... Es>
            constexpr SystemHandle(const System<Es...> system) noexcept // NOLINT
                : handle{ reinterpret_cast<Handle>(system) } {}

            [[nodiscard]] constexpr operator Handle() const noexcept { // NOLINT
                return handle;
            }
        };
    }

    template<std::convertible_to<detail::SystemHandle> auto...>
    struct SystemSet {};
}

template<>
struct std::hash<terra::core::detail::SystemHandle> {
    [[nodiscard]] static constexpr auto operator()(
        const terra::core::detail::SystemHandle handle
    ) noexcept -> std::size_t {
        return std::hash<terra::core::detail::SystemHandle::Handle>{}(handle.handle);
    }
};

namespace terra::core {
    class App;
    class World;

    struct TERRA_CORE_API Before {
        detail::SystemHandle system;
    };

    struct TERRA_CORE_API After {
        detail::SystemHandle system;
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
        using RunCondition = ConstExtractorFunction<bool, Es...>;
    }

    template<typename T>
    struct is_run_condition : std::bool_constant<requires(T condition) {
            wrap_const_extractor_function(condition);
        }> {};

    template<typename T>
    constexpr bool is_run_condition_v = is_run_condition<T>::value;

    template<typename T>
    concept RunCondition = is_run_condition_v<T>;

    template<typename T>
    struct is_system_option : std::bool_constant<
            is_run_condition_v<T> ||
            std::is_convertible_v<T, SystemOrdering>
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
                    .function = wrap_extractor_function(system),
                    .condition = {},
                },
                .orderings = {},
            };

            ([&] {
                if constexpr(is_run_condition_v<decltype(options)>) {
                    metadata.function.condition = wrap_const_extractor_function(options);
                } else if constexpr(std::is_convertible_v<decltype(options), SystemOrdering>) {
                    metadata.orderings.emplace_back(std::forward<decltype(options)>(options));
                }
            }(), ...);

            m_systems_metadata.try_emplace(system, std::move(metadata));
        }

        template<Extractor... Es>
        auto configure(const System<Es...> system, SystemOption auto&&... options) -> void {
            configure_handle(
                system,
                std::forward<decltype(options)>(options)...
            );
        }

        template<std::convertible_to<detail::SystemHandle> auto... Handles>
        auto configure_set(const SystemSet<Handles...>&, const SystemOption auto&... options) -> void {
            ([&] {
                configure_handle(Handles, options...);
            }(), ...);
        }

        auto build() -> void;
        auto run(World& world, App& app) -> void;

    private:
        std::vector<SystemFunction> m_systems{};
        std::unordered_map<detail::SystemHandle, SystemMetadata> m_systems_metadata{};

        auto configure_handle(const detail::SystemHandle handle, SystemOption auto&&... options) -> void {
            const auto it = m_systems_metadata.find(handle);
            if(it == m_systems_metadata.end()) {
                return;
            }

            auto& [function, orderings] = it->second;

            ([&] {
                if constexpr(is_run_condition_v<decltype(options)>) {
                    if(function.condition) {
                        function.condition = [
                                old_condition = std::move(function.condition),
                                new_condition = wrap_const_extractor_function(options)
                            ](const World& world, const App& app) mutable -> bool {
                                return old_condition(world, app) && new_condition(world, app);
                            };
                    } else {
                        function.condition = wrap_const_extractor_function(options);
                    }
                } else if constexpr(std::is_convertible_v<decltype(options), SystemOrdering>) {
                    orderings.emplace_back(std::forward<decltype(options)>(options));
                }
            }(), ...);
        }
    };
}
