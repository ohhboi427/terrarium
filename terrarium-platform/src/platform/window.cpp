#include <platform/window.hpp>
#include <terrarium/platform/window.hpp>

#include <glad/gl.h>

namespace terra::platform {
    using namespace core;

    Window::Window(const std::string_view title, const i32 width, const i32 height) noexcept {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

        m_handle = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_OPENGL);

        SDL_GLContextState* context = SDL_GL_CreateContext(WindowHandle{ *this });
        SDL_GL_MakeCurrent(WindowHandle{ *this }, context);

        gladLoadGL(SDL_GL_GetProcAddress);
    }

    Window::~Window() noexcept {
        SDL_DestroyWindow(WindowHandle{ *this });
    }

    WindowHandle::WindowHandle(const Window& window) noexcept
        : m_handle{ static_cast<SDL_Window*>(window.m_handle) } {}
}
