#pragma once

#include <terrarium/core/base.hpp>

#include <type_traits>

namespace terra::core {
    struct TERRA_CORE_API ITag {};

    template<typename T>
    struct is_tag : std::is_base_of<ITag, std::remove_cvref_t<T>> {};

    template<typename T>
    constexpr bool is_tag_v = is_tag<T>::value;

    template<typename T>
    concept Tag = is_tag<T>::value;
}
