#pragma once

#include <terrarium/defines.hpp>

#include <string_view>

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
}
