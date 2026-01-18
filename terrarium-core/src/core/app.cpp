#include <terrarium/core/app.hpp>

#include <core/time.hpp>
#include <core/debug/assert.hpp>

namespace terra::core {
    App::App() {
        m_event_bus = std::make_unique<EventBus>();
        m_task_pool = std::make_unique<TaskPool>(8U);

        add_plugin(clock_plugin);
    }

    auto App::run() -> void {
        register_crash_handler();

        m_event_bus->register_listener<AppQuitEvent>(
            [this](const AppQuitEvent&) noexcept -> bool {
                m_running.store(false, std::memory_order::release);

                return true;
            }
        );

        for(auto& schedule : m_schedules | std::views::values) {
            schedule.build();
        }

        m_running.store(true, std::memory_order::release);

        run_schedule<StartupTag>();

        while(true) {
            m_event_bus->flush();

            if(!m_running.load(std::memory_order::acquire)) {
                break;
            }

            m_world.flush();

            run_schedule<FrameBeginTag>();

            m_event_bus->flush();
            run_schedule<UpdateTag>();

            m_event_bus->flush();
            run_schedule<FrameEndTag>();
        }

        m_event_bus->flush();
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
