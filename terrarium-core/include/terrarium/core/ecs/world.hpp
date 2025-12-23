#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/debug/assert.hpp>
#include <terrarium/core/ecs/extractor.hpp>
#include <terrarium/core/ecs/resource.hpp>

#include <functional>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace terra::core {
    class TERRA_CORE_API World {
    public:
        World() = default;
        World(World&&) noexcept = delete;
        World(const World&) = delete;

        template<Resource R, typename... Args>
            requires std::conjunction_v<
                std::is_constructible<R, Args...>,
                std::negation<std::is_const<std::remove_reference_t<R>>>
            >
        auto make_resource(Args&&... args) -> void {
            using Inner = std::conditional_t<
                std::is_reference_v<R>,
                std::reference_wrapper<std::remove_reference_t<R>>,
                R
            >;

            m_resources.try_emplace(
                typeid(Inner),
                make_unique_any<Inner>(std::forward<Args>(args)...)
            );
        }

        template<Resource R>
            requires std::negation_v<std::is_const<std::remove_reference_t<R>>>
        [[nodiscard]] decltype(auto) get_resource(this auto&& self) noexcept {
            using Inner = std::conditional_t<
                std::is_reference_v<R>,
                std::reference_wrapper<std::remove_reference_t<R>>,
                R
            >;

            const auto it = self.m_resources.find(typeid(Inner));
            TERRA_DEBUG_ASSERT(it != self.m_resources.end(), "Resource '{}' not found", typeid(Inner).name());

            using ReturnType = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                std::add_pointer_t<const Inner>,
                std::add_pointer_t<Inner>
            >;

            return *static_cast<ReturnType>(it->second.get());
        }

    private:
        std::unordered_map<std::type_index, UniqueAny> m_resources{};
    };

    template<typename>
    class Res;

    template<Resource R>
    class Res<R> {
    public:
        explicit Res(World& world) noexcept
            : m_object{ world.get_resource<std::remove_cv_t<R>>() } {}

        [[nodiscard]] auto operator*() const noexcept -> R& {
            return m_object;
        }

        [[nodiscard]] auto operator->() const noexcept -> R* {
            return &m_object;
        }

    protected:
        R& m_object;
    };

    template<Resource R>
    class Res<R&> {
    public:
        explicit Res(World& world) noexcept
            : m_object{ world.get_resource<std::remove_cv_t<R>&>().get() } {}

        [[nodiscard]] auto operator*() const noexcept -> R& {
            return m_object;
        }

        [[nodiscard]] auto operator->() const noexcept -> R* {
            return &m_object;
        }

    protected:
        R& m_object;
    };

    template<Resource R>
    struct IExtractor<Res<R>> {
        [[nodiscard]] static auto operator()(World& world) -> Res<R> {
            return Res<R>{ world };
        }
    };
}
