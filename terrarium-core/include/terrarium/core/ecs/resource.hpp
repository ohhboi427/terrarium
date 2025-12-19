#pragma once

#include <terrarium/core/base.hpp>

#include <type_traits>

namespace terra::core {
    struct TERRA_CORE_API IResource {};

    template<typename T>
    struct is_resource : std::is_base_of<IResource, T> {};

    template<typename T>
    constexpr bool is_resource_v = is_resource<T>::value;

    template<typename T>
    concept Resource = is_resource_v<T>;
}
