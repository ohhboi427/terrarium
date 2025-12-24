#include <terrarium/core/app.hpp>

#include <terrarium/core/task.hpp>

#include <core/debug/assert.hpp>

#include <SDL3/SDL.h>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ranges>

namespace terra::core {
    auto App::run() -> void {
        register_crash_handler();

        m_world.make_resource<EventDispatcher>(m_event_bus);
        m_event_bus.register_listener<AppQuitEvent>(
            [this](const AppQuitEvent&) noexcept -> bool {
                m_running.store(false, std::memory_order::release);

                return true;
            }
        );

        TaskPool task_pool{ 8U };
        m_world.make_resource<TaskPool&>(task_pool);

        for(auto& schedule : m_schedules | std::views::values) {
            schedule.build();
        }

        SDL_Init(SDL_INIT_VIDEO);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

        SDL_Window* window = SDL_CreateWindow("Terrarium", 1280, 720, SDL_WINDOW_OPENGL);

        SDL_GLContextState* context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, context);

        gladLoadGL(SDL_GL_GetProcAddress);

        m_running.store(true, std::memory_order::release);

        run_schedule<StartupTag>();

        while(true) {
            SDL_Event event{};
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                case SDL_EVENT_QUIT:
                    m_event_bus.dispatch(AppQuitEvent{});
                    break;

                default:
                    break;
                }
            }

            m_event_bus.process_queue();

            if(!m_running.load(std::memory_order::acquire)) {
                break;
            }

            run_schedule<UpdateTag>();

            static constexpr glm::vec4 CLEAR_COLOR{ 1.0F, 0.0F, 1.0F, 1.0F };
            glClearNamedFramebufferfv(0U, GL_COLOR, 0U, glm::value_ptr(CLEAR_COLOR));

            SDL_GL_SwapWindow(window);
        }

        run_schedule<ShutdownTag>();

        SDL_Quit();
    }
}
