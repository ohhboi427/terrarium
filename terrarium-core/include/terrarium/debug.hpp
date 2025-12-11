#pragma once

#include <terrarium/defines.hpp>

#include <format>
#include <string_view>
#include <utility>

namespace terra {
    enum class Severity : u8 {
        Trace = 0U,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    TERRA_API auto log_raw(std::string_view message) -> void;
    TERRA_API auto log(Severity severity, std::string_view message) -> void;

    template<std::formattable<char>... Args>
    auto log(const Severity severity, const std::format_string<Args...> fmt, Args&&... args) -> void {
        if constexpr(sizeof...(Args) == 0U) {
            log(severity, fmt.get());
        } else {
            log(severity, std::format(fmt, std::forward<Args>(args)...));
        }
    }

    template<std::formattable<char>... Args>
    auto trace(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Trace, fmt, std::forward<Args>(args)...);
    }

    template<std::formattable<char>... Args>
    auto debug(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Debug, fmt, std::forward<Args>(args)...);
    }

    template<std::formattable<char>... Args>
    auto info(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Info, fmt, std::forward<Args>(args)...);
    }

    template<std::formattable<char>... Args>
    auto warn(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Warn, fmt, std::forward<Args>(args)...);
    }

    template<std::formattable<char>... Args>
    auto error(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Error, fmt, std::forward<Args>(args)...);
    }

    template<std::formattable<char>... Args>
    auto fatal(const std::format_string<Args...> fmt, Args&&... args) -> void {
        log(Severity::Fatal, fmt, std::forward<Args>(args)...);
    }
}

#include <cstdlib>
#define TERRA_ASSERT(condition) \
    do { \
        if(!(condition)) { \
            ::terra::fatal("'" #condition "' evaluated to false (" __FILE_NAME__ ":" TERRA_STRINGIFY(__LINE__) ")"); \
            ::std::abort(); \
        } \
    } while(false)

#if not defined(NDEBUG)
#   define TERRA_DEBUG_ASSERT(condition) TERRA_ASSERT(condition)
#else
#   define TERRA_DEBUG_ASSERT(condition) do {} while (false)
#endif
