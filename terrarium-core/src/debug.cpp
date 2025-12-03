#include <terrarium/debug.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <mutex>
#include <print>

template<>
struct std::formatter<terra::Severity> {
    static constexpr auto parse(std::format_parse_context& context) noexcept -> decltype(context.begin()) {
        return context.begin();
    }

    static constexpr auto format(
        const terra::Severity severity,
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

namespace terra {
    auto log(const Severity severity, const std::string_view message) -> void {
        const auto timestamp = std::chrono::system_clock::now();

        static std::mutex s_mutex{};
        const std::scoped_lock lock{ s_mutex };

        return std::println("{:%FT%TZ} {} {}", timestamp, severity, message);
    }
}
