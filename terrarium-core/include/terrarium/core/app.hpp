#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/task.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace terra::core {
    class TERRA_CORE_API App {
    public:
        auto run() -> void;

        template<Extractor... Es>
        auto add_system(const System<Es...> system) -> void {
            m_systems.emplace_back([system](World& world) mutable -> void {
                system(IExtractor<Es>::operator()(world)...);
            });
        }

    private:
        std::unique_ptr<TaskPool> m_task_pool{};
        World m_world{};

        std::vector<std::move_only_function<void(World&)>> m_systems{};
    };
}
