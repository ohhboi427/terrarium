#include <terrarium/core/plugin.hpp>

#include <terrarium/core/app.hpp>

#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace terra::core {
    auto PluginLoader::load_path(const std::filesystem::path& path, App& app) -> void {
        if(path.extension() != ".dll") {
            return;
        }

        HMODULE handle = LoadLibraryW(path.c_str());
        if(!handle) {
            return;
        }

        using PluginFunc = void(*)(App&);
        if(const auto func = reinterpret_cast<PluginFunc>(GetProcAddress(handle, "terra_plugin")); func) {
            app.add_plugin(func);
        } else {
            FreeLibrary(handle);
        }
    }
}
