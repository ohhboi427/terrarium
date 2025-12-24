#include <terrarium/core/ecs/world.hpp>

namespace terra::core {
    auto World::flush() -> void {
        m_queue.flush(*this);
    }

    auto World::CommandQueue::flush(World& world) -> void {
        std::queue<Command> commands{};

        {
            commands.swap(*m_commands.lock());
        }

        while(!commands.empty()) {
            auto command = std::move(commands.front());
            commands.pop();

            command(world);
        }
    }

    Commands::Commands(World& world) noexcept
        : m_queue{ world.m_queue } {}

    auto IExtractor<Commands>::operator()(World& world) noexcept -> Commands {
        return Commands{ world };
    }
}
