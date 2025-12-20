#include <terrarium/core/app.hpp>
#include <terrarium/core/task.hpp>
#include <terrarium/core/debug/log.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <memory_resource>

using namespace terra::primitives;
using namespace terra::core;

auto hello_world(Res<TaskPoolView> pool) -> void {
    pool->submit(
        []([[maybe_unused]] std::pmr::memory_resource& scratch) -> void {
            info("Hello World");
        }
    ).wait();
}

auto terrarium_plugin(App& app) noexcept -> void {
    app.add_system(hello_world);
}

auto main() -> i32 {
    App{}
        .add_plugin(terrarium_plugin)
        .run();
}
