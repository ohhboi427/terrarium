#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/debug/assert.hpp>
#include <terrarium/core/ecs/resource.hpp>

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
            requires std::is_constructible_v<R, Args...>
        auto make_resource(Args&&... args) -> void {
            m_resources.try_emplace(typeid(R), make_unique_any<SharedMutex<R>>(std::forward<Args>(args)...));
        }

        template<Resource R>
            requires std::is_move_constructible_v<R>
        auto add_resource(R&& resource) -> void {
            make_resource<R>(std::forward<R>(resource));
        }

        template<Resource R>
        [[nodiscard]] decltype(auto) get_resource(this auto&& self) {
            const auto it = self.m_resources.find(typeid(R));
            TERRA_DEBUG_ASSERT(it != self.m_resources.end(), "Resource '{}' not found", typeid(R).name());

            using Ret = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                std::add_pointer_t<const SharedMutex<R>>,
                std::add_pointer_t<SharedMutex<R>>
            >;

            return *static_cast<Ret>(it->second.get());
        }

    private:
        std::unordered_map<std::type_index, UniqueAny> m_resources{};
    };
}
