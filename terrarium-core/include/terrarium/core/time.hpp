#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/service.hpp>
#include <terrarium/core/time.hpp>
#include <terrarium/core/ecs/extractor.hpp>
#include <terrarium/core/ecs/resource.hpp>

namespace terra::core {
    class App;
    class World;

    struct TERRA_CORE_API AppTime : IService {
        f64 delta_seconds   = 0.0F;
        f64 elapsed_seconds = 0.0F;
    };

    struct TERRA_CORE_API WorldTime : IResource {
        f64 time_scale      = 1.0F;
        f64 delta_seconds   = 0.0F;
        f64 elapsed_seconds = 0.0F;
    };

    struct TERRA_CORE_API Time {
        AppTime&   app_time;
        WorldTime& world_time;
    };

    template<>
    struct TERRA_CORE_API IExtractor<Time> {
        [[nodiscard]] static auto operator()(World& world, App& app) noexcept -> Time;
    };
}
