#pragma once

#include <terrarium/defines.hpp>

namespace terra::core {
    auto thread_pool_init(usize num_workers) -> void;
    auto thread_pool_destroy() -> void;
}
