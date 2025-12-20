#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/task.hpp>
#include <terrarium/core/ecs/system.hpp>
#include <terrarium/core/ecs/world.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace terra::core {
    class TERRA_CORE_API App {
    public:
        auto run() -> void;

        template<Extractor... Es>
        auto add_system(const System<Es...> system, std::convertible_to<SystemOrdering> auto&&... orderings) -> void {
            m_schedule.add_system(system, std::forward<decltype(orderings)>(orderings)...);
        }

    private:
        std::unique_ptr<TaskPool> m_task_pool{};

        World m_world{};
        Schedule m_schedule{};
    };
}
