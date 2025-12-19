#pragma once

#include <terrarium/core/debug/log.hpp>

#include <cstdlib>

#define TERRA_ASSERT(condition, ...) \
    do { \
        if(!(condition)) { \
            ::terra::core::fatal("'" #condition "' evaluated to false (" __FILE_NAME__ ":" TERRA_STRINGIFY(__LINE__) ") " __VA_ARGS__); \
            ::std::abort(); \
        } \
    } while(false)

#if not defined(NDEBUG)
#   define TERRA_DEBUG_ASSERT(...) TERRA_ASSERT(__VA_ARGS__)
#else
#   define TERRA_DEBUG_ASSERT(...) do {} while (false)
#endif
