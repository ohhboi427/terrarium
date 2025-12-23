#pragma once

#if defined(TERRA_BUILD_SHARED)
#   if defined(_MSC_VER)
#       define TERRA_EXPORT __declspec(dllexport)
#       define TERRA_IMPORT __declspec(dllimport)
#   else
#       define TERRA_EXPORT __attribute__((visibility("default")))
#       define TERRA_IMPORT
#   endif
#else
#   define TERRA_EXPORT
#   define TERRA_IMPORT
#endif

#define TERRA_STRINGIFY_IMPL(x) #x
#define TERRA_STRINGIFY(x) TERRA_STRINGIFY_IMPL(x)

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace std2 {
    template<typename T>
    struct remove_ref_const : std::type_identity<T> {};

    template<typename T>
    struct remove_ref_const<T&> : std::remove_const<T> {};

    template<typename T>
    using remove_ref_const_t = remove_ref_const<T>::type;

    template<typename T>
    struct remove_ref_volatile : std::type_identity<T> {};

    template<typename T>
    struct remove_ref_volatile<T&> : std::remove_volatile<T> {};

    template<typename T>
    using remove_ref_volatile_t = remove_ref_volatile<T>::type;

    template<typename T>
    struct remove_ref_cv : std::type_identity<T> {};

    template<typename T>
    struct remove_ref_cv<T&> : std::remove_cv<T> {};

    template<typename T>
    using remove_ref_cv_t = remove_ref_cv<T>::type;

    template<typename T>
    struct is_clean_type : std::is_same<std::remove_cvref_t<T>, T> {};

    template<typename T>
    constexpr bool is_clean_type_v = is_clean_type<T>::value;

    template<typename T, typename R, typename... Args>
    concept invocable_r = std::is_invocable_r_v<R, T, Args...>;
}

namespace terra {
    using UniqueAny = std::unique_ptr<void, void(*)(void*)>;

    template<typename T, typename... Args>
        requires std::conjunction_v<std::is_constructible<T, Args...>, std2::is_clean_type<T>>
    [[nodiscard]] constexpr auto make_unique_any(Args&&... args) -> UniqueAny {
        return {
            new T(std::forward<Args>(args)...),
            [](void* const ptr) noexcept -> void {
                delete static_cast<T*>(ptr);
            }
        };
    }

    template<typename... Ts>
    struct Visitor : Ts... {
        using Ts::operator()...;
    };
}

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
