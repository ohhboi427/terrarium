#include <terrarium/core/utils/locator.hpp>

namespace terra::core {
    Locator::~Locator() noexcept {
        while(!m_objects.empty()) {
            m_objects.pop_back();
        }
    }
}
