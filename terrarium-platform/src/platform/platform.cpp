#include <terrarium/platform/platform.hpp>

#include <terrarium/core/app.hpp>
#include <terrarium/core/service.hpp>

#include <SDL3/SDL.h>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace terra::platform {
    using namespace core;

    namespace {
        class SdlContext : IService {
        public:
            SdlContext() {
                SDL_Init(SDL_INIT_VIDEO);

                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

                m_handle = SDL_CreateWindow("Terrarium", 1280, 720, SDL_WINDOW_OPENGL);

                SDL_GLContextState* context = SDL_GL_CreateContext(m_handle);
                SDL_GL_MakeCurrent(m_handle, context);

                gladLoadGL(SDL_GL_GetProcAddress);
            }

            ~SdlContext() {
                SDL_Quit();
            }

            [[nodiscard]] constexpr operator SDL_Window*() const noexcept { // NOLINT
                return m_handle;
            }

        private:
            SDL_Window* m_handle{};
        };
    }

    auto poll_events([[maybe_unused]] Serv<SdlContext> context, Events events) noexcept -> void {
        SDL_Event event{};
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_EVENT_QUIT:
                events.dispatch(AppQuitEvent{});
                break;

            default:
                break;
            }
        }
    }

    auto swap_buffers(Serv<SdlContext> context) noexcept -> void {
        static constexpr glm::vec4 CLEAR_COLOR{ 1.0F, 0.0F, 1.0F, 1.0F };
        glClearNamedFramebufferfv(0U, GL_COLOR, 0U, glm::value_ptr(CLEAR_COLOR));

        SDL_GL_SwapWindow(*context);
    }

    auto platform_plugin(App& app) noexcept -> void {
        app
            .make_service<SdlContext>()
            .add_system<FrameBeginTag>(poll_events)
            .add_system<FrameEndTag>(swap_buffers);
    }
}
