#pragma once

#include <terrarium/core/base.hpp>

#include <type_traits>

namespace terra::core {
    struct TERRA_CORE_API IService {};

    template<typename T>
    struct is_service : std::is_base_of<IService, std::remove_cvref_t<T>> {};

    template<typename T>
    constexpr bool is_service_v = is_service<T>::value;

    template<typename T>
    concept Service = is_service_v<T>;
}
