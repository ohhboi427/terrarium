#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/ecs/extractor.hpp>

#include <concepts>
#include <functional>
#include <queue>
#include <thread>
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

    template<Event E, Extractor... Es>
        requires std2::is_clean_type_v<E>
    using Listener = bool(*)(const E&, Es...);

    class TERRA_CORE_API EventBus {
        template<Event E>
            requires std2::is_clean_type_v<E>
        using ListenerFunction = std::move_only_function<bool(const E&)>;
        using DeferredDispatch = std::move_only_function<void()>;

    public:
        EventBus() noexcept;
        EventBus(EventBus&&) noexcept = delete;
        EventBus(const EventBus&) = delete;

        template<Event E>
            requires std2::is_clean_type_v<E>
        auto register_listener(std::invocable<const E&> auto&& listener) -> void {
            auto [it, is_new] = m_listeners.try_emplace(
                typeid(E),
                make_unique_any<std::vector<ListenerFunction<E>>>()
            );

            auto& listeners = *static_cast<std::vector<ListenerFunction<E>>*>(it->second.get());
            listeners.emplace_back(std::forward<decltype(listener)>(listener));
        }

        auto dispatch(Event auto&& event) -> void {
            thread_local const auto thread_id = std::this_thread::get_id();
            if(thread_id == m_main_thread_id) {
                dispatch_immediate(event);

                return;
            }

            auto deferred_dispatches = m_deferred_dispatches.lock();
            deferred_dispatches->emplace(
                [this, event = std::forward<decltype(event)>(event)]() mutable -> void {
                    dispatch_immediate(event);
                }
            );
        }

        auto flush() -> void;

    private:
        std::unordered_map<std::type_index, UniqueAny> m_listeners{};

        std::thread::id m_main_thread_id;
        Mutex<std::queue<DeferredDispatch>> m_deferred_dispatches{};

        auto dispatch_immediate(const Event auto& event) -> void {
            using EventType = std::remove_cvref_t<decltype(event)>;

            const auto it = m_listeners.find(typeid(EventType));
            if(it == m_listeners.end()) {
                return;
            }

            auto& listeners = *static_cast<std::vector<ListenerFunction<EventType>>*>(it->second.get());
            for(auto& listener : listeners) {
                const auto handled = listener(event);
                if(handled) {
                    break;
                }
            }
        }
    };
}
