#include <terrarium/core/plugin.hpp>

#include <filesystem>
#include <utility>

namespace terra::core {
    PluginLoader::PluginLoader(std::filesystem::path&& discover_path)
        : m_discover_folder{ std::move(discover_path) } {}
}
