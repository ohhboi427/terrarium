#include <terrarium/core/app.hpp>

#include <terrarium/core/task.hpp>

#include <core/debug/assert.hpp>

namespace terra::core {
    auto App::run() -> void {
        register_crash_handler();

        m_event_bus = std::make_unique<EventBus>();
        m_event_bus->register_listener<AppQuitEvent>(
            [this](const AppQuitEvent&) noexcept -> bool {
                m_running.store(false, std::memory_order::release);

                return true;
            }
        );

        m_task_pool = std::make_unique<TaskPool>(8U);

        for(auto& schedule : m_schedules | std::views::values) {
            schedule.build();
        }

        m_running.store(true, std::memory_order::release);

        run_schedule<StartupTag>();

        while(true) {
            run_schedule<FrameBeginTag>();

            m_event_bus->flush();

            if(!m_running.load(std::memory_order::acquire)) {
                break;
            }

            m_world.flush();
            run_schedule<UpdateTag>();

            run_schedule<FrameEndTag>();
        }

        run_schedule<ShutdownTag>();
    }

    Events::Events(App& app) noexcept
        : m_bus{ *app.m_event_bus } {}

    auto IExtractor<Events>::operator()(World&, App& app) noexcept -> Events {
        return Events{ app };
    }

    Tasks::Tasks(App& app) noexcept
        : m_pool{ *app.m_task_pool } {}

    auto IExtractor<Tasks>::operator()(World&, App& app) noexcept -> Tasks {
        return Tasks{ app };
    }
}
