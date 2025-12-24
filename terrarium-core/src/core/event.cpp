#include <terrarium/core/event.hpp>

namespace terra::core {
    EventBus::EventBus() noexcept
        : m_main_thread_id{ std::this_thread::get_id() } {}

    auto EventBus::flush() -> void {
        auto deferred_dispatches = m_deferred_dispatches.lock();
        while(!deferred_dispatches->empty()) {
            auto dispatch = std::move(deferred_dispatches->front());
            deferred_dispatches->pop();

            dispatch();
        }
    }

    EventDispatcher::EventDispatcher(EventBus& bus) noexcept
        : m_bus{ bus } {}
}
