#pragma once

#include <terrarium/platform/base.hpp>

namespace terra::core {
    class App;
}

namespace terra::platform {
    auto TERRA_PLATFORM_API platform_plugin(core::App& app) noexcept -> void;
}
