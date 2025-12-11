#pragma once

#include <terrarium/core/debug/log.hpp>

#include <cstdlib>
#define TERRA_ASSERT(condition) \
    do { \
        if(!(condition)) { \
            ::terra::core::fatal("'" #condition "' evaluated to false (" __FILE_NAME__ ":" TERRA_STRINGIFY(__LINE__) ")"); \
            ::std::abort(); \
        } \
    } while(false)

#if not defined(NDEBUG)
#   define TERRA_DEBUG_ASSERT(condition) TERRA_ASSERT(condition)
#else
#   define TERRA_DEBUG_ASSERT(condition) do {} while (false)
#endif
