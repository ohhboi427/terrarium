#pragma once

#include <terrarium/core/base.hpp>

#include <filesystem>
#include <type_traits>

namespace terra::core {
    class App;

    template<typename T>
    struct is_plugin : std::is_invocable<T, App&> {};

    template<typename T>
    constexpr bool is_plugin_v = is_plugin<T>::value;

    template<typename T>
    concept Plugin = is_plugin_v<T>;

    class TERRA_CORE_API PluginLoader {
    public:
        explicit PluginLoader(std::filesystem::path&& discover_path) noexcept;

        auto operator()(App& app) const -> void;

    private:
        std::filesystem::path m_discover_folder;

        static auto load_path(const std::filesystem::path& path, App& app) -> void;
    };
}
