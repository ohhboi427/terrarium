#include <terrarium/platform/platform.hpp>

#include <terrarium/core/app.hpp>
#include <terrarium/platform/input.hpp>
#include <terrarium/platform/window.hpp>

#include <platform/sdl_context.hpp>
#include <platform/window.hpp>

#include <SDL3/SDL.h>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace terra::platform {
    using namespace core;

    auto poll_events(Events events) noexcept -> void {
        SDL_Event event{};
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_EVENT_QUIT:
                events.dispatch(AppQuitEvent{});
                break;

            case SDL_EVENT_KEY_UP: [[fallthrough]];
            case SDL_EVENT_KEY_DOWN:
                events.dispatch(
                    KeyEvent{
                        .key = static_cast<Keys>(event.key.key),
                        .action = static_cast<Actions>(event.key.down),
                        .mods = static_cast<Modifiers>(event.key.mod),
                    }
                );
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP: [[fallthrough]];
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                events.dispatch(
                    MouseButtonEvent{
                        .button = static_cast<MouseButtons>(event.button.button),
                        .action = static_cast<Actions>(event.button.down),
                    }
                );
                break;

            default:
                break;
            }
        }
    }

    auto swap_buffers(Serv<Window> window) noexcept -> void {
        static constexpr glm::vec4 CLEAR_COLOR{ 1.0F, 0.0F, 1.0F, 1.0F };
        glClearNamedFramebufferfv(0U, GL_COLOR, 0U, glm::value_ptr(CLEAR_COLOR));

        SDL_GL_SwapWindow(WindowHandle{ *window });
    }

    auto platform_plugin(App& app) noexcept -> void {
        app.make_service<SdlContext>();
        app.make_service<Window>("Terrarium", 1280, 720);

        app.add_system<FrameBeginTag>(poll_events);
        app.add_system<FrameEndTag>(swap_buffers);
    }
}
