#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/ecs/resource.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <concepts>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace terra::core {
    struct TERRA_CORE_API IEvent {};

    template<typename T>
    struct is_event : std::is_base_of<IEvent, std::remove_cvref_t<T>> {};

    template<typename T>
    constexpr bool is_event_v = is_event<T>::value;

    template<typename T>
    concept Event = is_event_v<T>;

    class TERRA_CORE_API EventBus {
        template<Event E>
        using Listener = std::move_only_function<void(const E&)>;

    public:
        EventBus() noexcept = default;
        EventBus(EventBus&&) noexcept = delete;
        EventBus(const EventBus&) = delete;

        template<Event E>
        auto register_listener(std::invocable<const E&> auto&& listener) -> void {
            auto [it, is_new] = m_listeners.try_emplace(typeid(E), make_unique_any<std::vector<Listener<E>>>());
            auto& listeners = *static_cast<std::vector<Listener<E>>*>(it->second.get());

            listeners.emplace_back(std::forward<decltype(listener)>(listener));
        }

        auto dispatch(Event auto&& event) -> void {
            dispatch_immediate(event);
        }

    private:
        std::unordered_map<std::type_index, UniqueAny> m_listeners{};

        auto dispatch_immediate(const Event auto& event) -> void {
            const auto it = m_listeners.find(typeid(event));
            if(it == m_listeners.end()) {
                return;
            }

            auto& listeners = *static_cast<std::vector<Listener<decltype(event)>>*>(it->second.get());
            for(auto& listener : listeners) {
                listener(event);
            }
        }
    };

    class TERRA_CORE_API EventDispatcher : IResource {
    public:
        explicit EventDispatcher(EventBus& bus) noexcept;

        auto dispatch(const Event auto& event) -> void {
            m_bus.dispatch(event);
        }

    private:
        EventBus& m_bus;
    };
}
