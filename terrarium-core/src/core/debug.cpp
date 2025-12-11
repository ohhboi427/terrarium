#include <core/debug.hpp>
#include <terrarium/core/debug.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>

template<>
struct std::formatter<terra::core::Severity> {
    static constexpr auto parse(std::format_parse_context& context) noexcept -> decltype(context.begin()) {
        return context.begin();
    }

    static constexpr auto format(
        const terra::core::Severity severity,
        std::format_context& context
    ) -> decltype(context.out()) {
        using namespace std::string_view_literals;

        const auto index = std::to_underlying(severity);
        static constexpr std::array SEVERITY_NAMES = {
            "TRACE"sv,
            "DEBUG"sv,
            "INFO"sv,
            "WARN"sv,
            "ERROR"sv,
            "FATAL"sv,
        };

        return std::ranges::copy(SEVERITY_NAMES[index], context.out()).out;
    }
};

namespace terra::core {
    auto log_raw(const std::string_view message) -> void {
        static std::mutex s_mutex{};
        const std::scoped_lock lock{ s_mutex };

        std::cout << message << std::endl;
    }

    auto log(const Severity severity, const std::string_view message) -> void {
        const auto timestamp = std::chrono::system_clock::now();

        log_raw(std::format("{:%FT%TZ} {} {}", timestamp, severity, message));
    }
}
