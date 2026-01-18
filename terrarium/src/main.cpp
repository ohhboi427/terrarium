#include <terrarium/core/app.hpp>
#include <terrarium/core/time.hpp>
#include <terrarium/core/debug/log.hpp>
#include <terrarium/platform/input.hpp>
#include <terrarium/platform/platform.hpp>

using namespace terra::primitives;

using namespace terra::core;
using namespace terra::platform;

auto hello_world() -> void {
    info("Hello, World!");
}

auto fps(const Time time) -> void {
    info("FPS: {}", 1.0 / time.app_time.delta_seconds);
}

auto terrarium_plugin(App& app) noexcept -> void {
    app.add_system<StartupTag>(hello_world);
    app.add_system<UpdateTag>(fps);
    app.register_listener(
        +[](const KeyEvent& event, Events events) noexcept -> bool {
            if(event.key == Keys::Escape && event.mods.has_any(Modifiers::Shift)) {
                events.dispatch(AppQuitEvent{});
            }

            return false;
        }
    );
}

auto main() -> i32 {
    App{}
        .add_plugin(platform_plugin)
        .add_plugin(terrarium_plugin)
        .run();
}
