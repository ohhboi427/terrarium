#include <terrarium/core/plugin.hpp>

#include <filesystem>
#include <utility>

namespace terra::core {
    PluginLoader::PluginLoader(std::filesystem::path&& discover_path) noexcept
        : m_discover_folder{ std::move(discover_path) } {}

    auto PluginLoader::operator()(App& app) const -> void {
        if(!std::filesystem::exists(m_discover_folder)) {
            return;
        }

        for(const auto& entry : std::filesystem::directory_iterator{ m_discover_folder }) {
            load_path(entry.path(), app);
        }
    }
}
