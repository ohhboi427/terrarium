#pragma once

#include <terrarium/defines.hpp>

#if defined(TERRA_PLATFORM_EXPORT)
#   define TERRA_PLATFORM_API TERRA_EXPORT
#else
#   define TERRA_PLATFORM_API TERRA_IMPORT
#endif
