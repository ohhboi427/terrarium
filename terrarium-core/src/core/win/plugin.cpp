#include <terrarium/core/plugin.hpp>

#include <terrarium/core/app.hpp>

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
        if(const auto func = reinterpret_cast<PluginFunc>(GetProcAddress(handle, ENTRY_POINT.data())); func) {
            app.add_plugin(func);
        } else {
            FreeLibrary(handle);
        }
    }
}
