#pragma once

#include <terrarium/core/service.hpp>
#include <terrarium/platform/base.hpp>

#include <string_view>

namespace terra::platform {
    class TERRA_PLATFORM_API Window : core::IService {
        friend class WindowHandle;

    public:
        explicit Window(std::string_view title, i32 width, i32 height) noexcept;
        ~Window() noexcept;

        Window(Window&&) noexcept = delete;
        Window(const Window&) = delete;

    private:
        void* m_handle;
    };
}
