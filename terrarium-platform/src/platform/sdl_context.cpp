#include <platform/sdl_context.hpp>

#include <SDL3/SDL.h>

namespace terra::platform {
    using namespace core;

    SdlContext::SdlContext() noexcept {
        SDL_Init(SDL_INIT_VIDEO);
    }

    SdlContext::~SdlContext() noexcept {
        SDL_Quit();
    }
}
