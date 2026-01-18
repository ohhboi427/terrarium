#include <core/time.hpp>
#include <terrarium/core/time.hpp>

#include <terrarium/core/app.hpp>
#include <terrarium/core/ecs/world.hpp>

namespace terra::core {
    namespace {
        struct Timer : IService {
            using Clock = std::chrono::high_resolution_clock;

            Clock::time_point last;
            Clock::time_point current;

            auto tick() noexcept -> void;
            auto reset() noexcept -> void;
        };

        auto clock_init_system(Commands commands) noexcept -> void {
            commands.make_resource<WorldTime>();
        }

        auto clock_tick_system(Serv<Timer> timer, Serv<AppTime> app_time, Res<WorldTime> world_time) noexcept -> void {
            using Second = std::chrono::duration<f64>;

            timer->tick();

            app_time->delta_seconds   = std::chrono::duration_cast<Second>(timer->current - timer->last).count();
            app_time->elapsed_seconds += app_time->delta_seconds;

            world_time->delta_seconds   = app_time->delta_seconds * world_time->time_scale;
            world_time->elapsed_seconds += world_time->delta_seconds;
        }
    }

    auto Timer::tick() noexcept -> void {
        last    = current;
        current = Clock::now();
    }

    auto Timer::reset() noexcept -> void {
        last    = Clock::now();
        current = last;
    }

    auto clock_plugin(App& app) -> void {
        auto& timer = app.make_service<Timer>();
        timer.reset();

        app.make_service<AppTime>();
        app.add_system<StartupTag>(clock_init_system);
        app.add_system<FrameBeginTag>(clock_tick_system);
    }

    auto IExtractor<Time>::operator()(World& world, App& app) noexcept -> Time {
        return Time{ .app_time = app.get_service<AppTime>(), .world_time = world.get_resource<WorldTime>() };
    }
}
