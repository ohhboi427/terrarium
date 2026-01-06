#pragma once

#include <terrarium/core/service.hpp>
#include <terrarium/platform/base.hpp>

#include <SDL3/SDL.h>

#include <bitset>
#include <utility>

namespace terra::core {
    class Events;
}

namespace terra::platform {
    struct InputState : core::IService {
        std::bitset<SDL_SCANCODE_COUNT> key_states;
        std::bitset<5> mouse_button_states;
        std::pair<i32, i32> mouse_position;
        std::pair<i32, i32> mouse_delta;

        auto handle_event(const SDL_Event& event, core::Events& events) noexcept -> void;
        auto handle_key_event(const SDL_KeyboardEvent& event, core::Events& events) noexcept -> void;
        auto handle_mouse_button_event(const SDL_MouseButtonEvent& event, core::Events& events) noexcept -> void;
        auto handle_mouse_move_event(const SDL_MouseMotionEvent& event, core::Events& events) noexcept -> void;
        auto handle_scroll_event(const SDL_MouseWheelEvent& event, core::Events& events) noexcept -> void;
    };
}
