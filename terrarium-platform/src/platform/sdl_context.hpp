#pragma once

#include <terrarium/core/service.hpp>

namespace terra::platform {
    class SdlContext : core::IService {
    public:
        SdlContext() noexcept;
        ~SdlContext() noexcept;

        SdlContext(SdlContext&&) noexcept = delete;
        SdlContext(const SdlContext&) = delete;
    };
}
