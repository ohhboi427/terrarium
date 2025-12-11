#pragma once

#include <terrarium/defines.hpp>

#if defined(TERRA_CORE_EXPORT)
#   define TERRA_CORE_API TERRA_EXPORT
#else
#   define TERRA_CORE_API TERRA_IMPORT
#endif
