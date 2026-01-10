#include <terrarium/core/app.hpp>
#include <terrarium/core/debug/log.hpp>
#include <terrarium/platform/input.hpp>
#include <terrarium/platform/platform.hpp>

using namespace terra::primitives;

using namespace terra::core;
using namespace terra::platform;

auto hello_world() -> void {
    info("Hello, World!");
}

auto terrarium_plugin(App& app) noexcept -> void {
    app.add_system<StartupTag>(hello_world);
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
