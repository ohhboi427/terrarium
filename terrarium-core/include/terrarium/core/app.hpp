#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/ecs/world.hpp>

namespace terra::core {
    class TERRA_CORE_API App {
    public:
        auto run() -> void;

    private:
        World m_world{};
    };
}
