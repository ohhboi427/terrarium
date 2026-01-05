#pragma once

#include <terrarium/core/app.hpp>
#include <terrarium/platform/input.hpp>

#include <SDL3/SDL.h>

namespace terra::platform {
    class InputHandler {
    public:
        explicit InputHandler(InputState& state, core::Events& events) noexcept;

        auto handle_event(const SDL_Event& event) const noexcept -> void;

    private:
        InputState& m_state;
        core::Events& m_events;

        auto handle_key_event(const SDL_KeyboardEvent& event) const noexcept -> void;
        auto handle_mouse_button_event(const SDL_MouseButtonEvent& event) const noexcept -> void;
        auto handle_mouse_move_event(const SDL_MouseMotionEvent& event) const noexcept -> void;
        auto handle_scroll_event(const SDL_MouseWheelEvent& event)const noexcept -> void;
    };
}
