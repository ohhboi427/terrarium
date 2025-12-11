#pragma once

#if defined(TERRA_BUILD_SHARED)
#   if defined(_MSC_VER)
#       if defined(TERRA_CORE_EXPORT)
#           define TERRA_CORE_API __declspec(dllexport)
#       else
#           define TERRA_CORE_API __declspec(dllimport)
#       endif
#   else
#       if defined(TERRA_EXPORT)
#           define TERRA_CORE_API __attribute__((visibility("default")))
#       else
#           define TERRA_CORE_API
#       endif
#   endif
#else
#   define TERRA_CORE_API
#endif

#define TERRA_STRINGIFY_IMPL(x) #x
#define TERRA_STRINGIFY(x) TERRA_STRINGIFY_IMPL(x)

#include <cstddef>
#include <cstdint>

namespace terra::inline primitives {
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

    using byte = std::byte;
}

namespace terra::inline literals::inline memory_literals {
    [[nodiscard]] consteval auto operator""_b(const unsigned long long size) noexcept -> usize {
        return size;
    }

    [[nodiscard]] consteval auto operator""_kb(const unsigned long long size) noexcept -> usize {
        return size * 1024U;
    }

    [[nodiscard]] consteval auto operator""_mb(const unsigned long long size) noexcept -> usize {
        return size * 1024U * 1024U;
    }

    [[nodiscard]] consteval auto operator""_gb(const unsigned long long size) noexcept -> usize {
        return size * 1024U * 1024U * 1024U;
    }
}
