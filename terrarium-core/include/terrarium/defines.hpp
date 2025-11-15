#pragma once

#if defined(TERRA_BUILD_SHARED)
#   if defined(_MSC_VER)
#       if defined(TERRA_EXPORT)
#           define TERRA_API __declspec(dllexport)
#       else
#           define TERRA_API __declspec(dllimport)
#       endif
#   else
#       if defined(TERRA_EXPORT)
#           define TERRA_API __attribute__((visibility("default")))
#       else
#           define TERRA_API
#       endif
#   endif
#else
#   define TERRA_API
#endif

#include <cstdint>

namespace terra::core::inline primitives {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using usize = std::uintptr_t;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using isize = std::intptr_t;

    using f32 = float;
    using f64 = double;
}
