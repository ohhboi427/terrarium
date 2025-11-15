#pragma once

#if defined(TERRA_BUILD_SHARED)
#   if defined(_MSC_VER)
#       if defined(TERRA_EXPORT)
#           define TERRA_API __declspec(dllexport)
#       else
#           define TERRA_API __declspec(dllimport)
#       endif
#   else
#       if defined(TERRA_EXPORT)
#           define TERRA_API __attribute__((visibility("default")))
#       else
#           define TERRA_API
#       endif
#   endif
#else
#   define TERRA_API
#endif
