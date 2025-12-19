#pragma once

#include <terrarium/core/ecs/world.hpp>

namespace terra::core {
    template<Extractor... Es>
    using System = void(*)(Es...);
}
