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

auto number_print2(const Res<const Number> number) -> void {
    info("Number2: {}", number->value);
}

auto terrarium_plugin(App& app) noexcept -> void {
    app.add_system<StartupTag>(hello_world);
    app.add_system<UpdateTag>(number_print);
    app.add_system<UpdateTag>(number_print2);
    app.configure_set(
        SystemSet<number_print, number_print2>{},
        +[](Res<const Number> number) {
            return number->value < 10;
        }
    );
}

auto main() -> i32 {
    App{}
        .add_plugin(terrarium_plugin)
        .add_plugin(PluginLoader{ "mods" })
        .run();
}
