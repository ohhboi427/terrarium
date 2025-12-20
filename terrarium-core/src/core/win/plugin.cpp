#include <terrarium/core/plugin.hpp>

#include <terrarium/core/app.hpp>

#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace terra::core {
    auto PluginLoader::operator()(App& app) const -> void {
        if(!std::filesystem::exists(m_discover_folder)) {
            return;
        }

        for(const auto& entry : std::filesystem::directory_iterator{ m_discover_folder }) {
            if(entry.path().extension() != ".dll") {
                continue;
            }

            HMODULE handle = LoadLibraryW(entry.path().c_str());
            if(!handle) {
                continue;
            }

            using PluginFunc = void(*)(App&);
            if(const auto func = reinterpret_cast<PluginFunc>(GetProcAddress(handle, "terra_plugin")); func) {
                app.add_plugin(func);
            }
        }
    }
}
