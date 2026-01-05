#pragma once

#include <terrarium/core/base.hpp>

#include <algorithm>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <vector>

namespace terra::core {
    class TERRA_CORE_API Locator {
    public:
        Locator() = default;

        Locator(Locator&&) noexcept = delete;
        Locator(const Locator&) = delete;

        template<typename T, typename... Args>
            requires std2::is_clean_type_v<std::remove_reference_t<T>> && std::is_constructible_v<T, Args...>
        auto make_object(Args&&... args) -> std::add_lvalue_reference_t<T> {
            using Inner = std::conditional_t<
                std::is_reference_v<T>,
                std::reference_wrapper<T>,
                T
            >;

            auto& [type, object] = m_objects.emplace_back(
                typeid(Inner),
                make_unique_any<Inner>(std::forward<Args>(args)...)
            );

            return *static_cast<Inner*>(object.get());
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

            const auto it = std::ranges::find_if(
                self.m_objects,
                [](const std::pair<std::type_index, UniqueAny>& object) noexcept -> bool {
                    return object.first == typeid(Inner);
                }
            );

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

            const auto it = std::ranges::find_if(
                m_objects,
                [](const std::pair<std::type_index, UniqueAny>& object) noexcept -> bool {
                    return object.first == typeid(Inner);
                }
            );

            return it != m_objects.end();
        }

    private:
        std::vector<std::pair<std::type_index, UniqueAny>> m_objects{};
    };
}
