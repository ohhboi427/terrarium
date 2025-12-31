#pragma once

#include <terrarium/core/base.hpp>

#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace terra::core {
    class TERRA_CORE_API Locator {
    public:
        Locator() = default;

        Locator(Locator&&) noexcept = delete;
        Locator(const Locator&) = delete;

        template<typename T, typename... Args>
            requires std2::is_clean_type_v<std::remove_reference_t<T>> && std::is_constructible_v<T, Args...>
        auto make_object(Args&&... args) -> void {
            using Inner = std::conditional_t<
                std::is_reference_v<T>,
                std::reference_wrapper<T>,
                T
            >;

            m_objects.try_emplace(
                typeid(Inner),
                make_unique_any<Inner>(std::forward<Args>(args)...)
            );
        }

        template<typename T>
            requires std2::is_clean_type_v<std::remove_reference_t<T>>
        [[nodiscard]] decltype(auto) get_object(this auto&& self) noexcept {
            using Inner = std::conditional_t<
                std::is_reference_v<T>,
                std::reference_wrapper<T>,
                T
            >;

            using ObjectType = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                const std::remove_reference_t<T>,
                std::remove_reference_t<T>
            >;

            const auto it = self.m_objects.find(typeid(Inner));
            if(it == self.m_objects.end()) {
                return static_cast<ObjectType*>(nullptr);
            }

            ObjectType& object = *static_cast<Inner*>(it->second.get());
            return &object;
        }

        template<typename T>
            requires std2::is_clean_type_v<std::remove_reference_t<T>>
        [[nodiscard]] auto has_object() const noexcept {
            using Inner = std::conditional_t<
                std::is_reference_v<T>,
                std::reference_wrapper<T>,
                T
            >;

            return m_objects.contains(typeid(Inner));
        }

    private:
        std::unordered_map<std::type_index, UniqueAny> m_objects{};
    };
}
