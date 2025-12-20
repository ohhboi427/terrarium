#include <terrarium/core/event.hpp>

namespace terra::core {
    EventDispatcher::EventDispatcher(EventBus& bus) noexcept
        : m_bus{ bus } {}
}
