#pragma once

#include <SDL3/SDL.h>

namespace terra::platform {
    class Window;

    class WindowHandle {
    public:
        explicit WindowHandle(const Window& window) noexcept;

        constexpr explicit WindowHandle(SDL_Window* handle) noexcept
            : m_handle{ handle } {}

        WindowHandle(WindowHandle&&) noexcept = delete;
        WindowHandle(const WindowHandle&) = delete;

        [[nodiscard]] constexpr operator SDL_Window*() const noexcept { // NOLINT
            return m_handle;
        }

    private:
        SDL_Window* m_handle = nullptr;
    };
}
