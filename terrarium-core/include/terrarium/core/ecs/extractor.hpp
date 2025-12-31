#pragma once

#include <concepts>
#include <functional>
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

    template<typename R, Extractor... Es>
    using ExtractorFunction = R(*)(Es...);

    template<typename R, Extractor... Es>
    [[nodiscard]] constexpr auto wrap_extractor_function(const ExtractorFunction<R, Es...> function) {
        return [function](World& world, App& app) mutable {
            if constexpr(std::is_same_v<R, void>) {
                std::invoke(function, IExtractor<Es>::operator()(world, app)...);
            } else {
                return std::invoke(function, IExtractor<Es>::operator()(world, app)...);
            }
        };
    }

    template<typename T>
    struct is_const_extractor : std::bool_constant<requires(const World& world, const App& app) {
            { IExtractor<T>::operator()(world, app) } -> std::same_as<T>;
        }> {};

    template<typename T>
    constexpr bool is_const_extractor_v = is_const_extractor<T>::value;

    template<typename T>
    concept ConstExtractor = is_const_extractor_v<T>;

    template<typename R, ConstExtractor... Es>
    using ConstExtractorFunction = R(*)(Es...);

    template<typename R, ConstExtractor... Es>
    [[nodiscard]] constexpr auto wrap_const_extractor_function(const ConstExtractorFunction<R, Es...> function) {
        return [function](const World& world, const App& app) mutable {
            if constexpr(std::is_same_v<R, void>) {
                std::invoke(function, IExtractor<Es>::operator()(world, app)...);
            } else {
                return std::invoke(function, IExtractor<Es>::operator()(world, app)...);
            }
        };
    }
}
