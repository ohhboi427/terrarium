#include <terrarium/core/ecs/world.hpp>

namespace terra::core {
    auto World::process_queue() -> void {
        m_queue.process_queue(*this);
    }

    auto World::CommandQueue::process_queue(World& world) -> void {
        auto commands = m_commands.lock();
        while(!commands->empty()) {
            auto command = std::move(commands->front());
            commands->pop();

            command(world);
        }
    }

    Commands::Commands(World& world) noexcept
        : m_queue{ world.m_queue } {}

    auto IExtractor<Commands>::operator()(World& world) noexcept -> Commands {
        return Commands{ world };
    }
}
