#include <terrarium/core/app.hpp>
#include <terrarium/core/plugin.hpp>
#include <terrarium/core/task.hpp>
#include <terrarium/core/debug/log.hpp>
#include <terrarium/core/ecs/world.hpp>

using namespace terra::core;
using namespace terra::primitives;

struct Number : IResource {
    int value;
};

auto hello_world(Tasks tasks, Commands commands) -> void {
    tasks.submit(
        []([[maybe_unused]] std::pmr::memory_resource& scratch) -> void {
            info("Hello, World!");
        }
    ).wait();

    commands.make_resource<Number>(Number{ .value = 42 });
}

auto number_print(const Res<const Number> number) -> void {
    info("Number: {}", number->value);
}

auto terrarium_plugin(App& app) noexcept -> void {
    app.add_system<StartupTag>(hello_world);
    app.add_system<UpdateTag>(
        number_print,
        [](const World&, const App&) noexcept -> bool {
            return false;
        }
    );
}

auto main() -> i32 {
    App{}
        .add_plugin(terrarium_plugin)
        .add_plugin(PluginLoader{ "mods" })
        .run();
}
