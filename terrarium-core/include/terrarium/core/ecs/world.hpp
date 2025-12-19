#pragma once

#include <terrarium/core/base.hpp>

namespace terra::core {
    class TERRA_CORE_API World {
    public:
        World() = default;
        World(World&&) noexcept = delete;
        World(const World&) = delete;
    };
}
