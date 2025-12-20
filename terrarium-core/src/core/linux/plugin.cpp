#include <terrarium/core/plugin.hpp>

#include <terrarium/core/app.hpp>

#include <filesystem>

#include <dlfcn.h>

namespace terra::core {
    auto PluginLoader::operator()(App& app) const -> void {
        if(!std::filesystem::exists(m_discover_folder)) {
            return;
        }

        for(const auto& entry : std::filesystem::directory_iterator{ m_discover_folder }) {
            if(entry.path().extension() != ".so") {
                continue;
            }

            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if(!handle) {
                continue;
            }

            using PluginFunc = void(*)(App&);
            if(const auto func = reinterpret_cast<PluginFunc>(dlsym(handle, "terra_plugin")); func) {
                app.add_plugin(func);
            }
        }
    }
}
