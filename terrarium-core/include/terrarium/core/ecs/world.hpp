#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/debug/assert.hpp>
#include <terrarium/core/ecs/resource.hpp>

#include <concepts>
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
            requires std::conjunction_v<std::is_constructible<R, Args...>, std2::is_clean_type<R>>
        auto make_resource(Args&&... args) -> void {
            m_resources.try_emplace(
                typeid(R),
                make_unique_any<SharedMutex<R>>(std::forward<Args>(args)...)
            );
        }

        template<Resource R>
            requires std2::is_clean_type_v<R>
        [[nodiscard]] decltype(auto) get_resource(this auto&& self) noexcept {
            const auto it = self.m_resources.find(typeid(R));
            TERRA_DEBUG_ASSERT(it != self.m_resources.end(), "Resource '{}' not found", typeid(R).name());

            using MutexType = SharedMutex<R>;
            using ReturnType = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                std::add_pointer_t<const MutexType>,
                std::add_pointer_t<MutexType>
            >;

            return *static_cast<ReturnType>(it->second.get());
        }

    private:
        std::unordered_map<std::type_index, UniqueAny> m_resources{};
    };

    template<typename>
    struct IExtractor;

    template<typename T>
    struct is_extractor : std::bool_constant<requires(World& world) {
            { IExtractor<T>::operator()(world) } -> std::same_as<T>;
        }> {};

    template<typename T>
    constexpr bool is_extractor_v = is_extractor<T>::value;

    template<typename T>
    concept Extractor = is_extractor_v<T>;

    template<Resource R>
        requires std::negation_v<std::is_reference<R>>
    class Res {
    public:
        explicit Res(World& world)
            : object{ world.get_resource<R>().lock() } {}

        [[nodiscard]] auto operator*() const noexcept -> R& {
            return object.operator*();
        }

        [[nodiscard]] auto operator->() const noexcept -> R* {
            return object.operator->();
        }

    private:
        LockGuard<R, typename SharedMutex<R>::Inner> object;
    };

    template<Resource R>
        requires std::negation_v<std::is_reference<R>>
    class Res<const R> {
    public:
        explicit Res(const World& world)
            : object{ world.get_resource<R>().shared_lock() } {}

        [[nodiscard]] auto operator*() const noexcept -> const R& {
            return object.operator*();
        }

        [[nodiscard]] auto operator->() const noexcept -> const R* {
            return object.operator->();
        }

    private:
        SharedLockGuard<R> object;
    };

    template<Resource R>
    struct IExtractor<Res<R>> {
        [[nodiscard]] static auto operator()(World& world) -> Res<R> {
            return Res<R>{ world };
        }
    };
}
