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
