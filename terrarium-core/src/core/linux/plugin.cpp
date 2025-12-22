#include <terrarium/core/plugin.hpp>

#include <terrarium/core/app.hpp>

#include <dlfcn.h>

namespace terra::core {
    auto PluginLoader::load_path(const std::filesystem::path& path, App& app) -> void {
        if(path.extension() != ".so") {
            return;
        }

        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if(!handle) {
            return;
        }

        using PluginFunc = void(*)(App&);
        if(const auto func = reinterpret_cast<PluginFunc>(dlsym(handle, "terra_plugin")); func) {
            app.add_plugin(func);
        } else {
            dlclose(handle);
        }
    }
}
