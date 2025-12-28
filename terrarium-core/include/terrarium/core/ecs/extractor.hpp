#pragma once

#include <concepts>
#include <type_traits>

namespace terra::core {
    class App;
    class World;

    template<typename>
    struct IExtractor;

    template<typename T>
    struct is_extractor : std::bool_constant<requires(World& world, App& app) {
            { IExtractor<T>::operator()(world, app) } -> std::same_as<T>;
        }> {};

    template<typename T>
    constexpr bool is_extractor_v = is_extractor<T>::value;

    template<typename T>
    concept Extractor = is_extractor_v<T>;
}
