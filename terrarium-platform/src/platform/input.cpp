#include <platform/input.hpp>
#include <terrarium/platform/input.hpp>

#include <SDL3/SDL.h>

namespace terra::platform {
    using namespace core;

    auto InputState::key_state(const Keys key) const noexcept -> Actions {
        return static_cast<Actions>(m_key_states.test(std::to_underlying(key)));
    }

    auto InputState::mouse_button_state(const MouseButtons button) const noexcept -> Actions {
        return static_cast<Actions>(m_mouse_button_states.test(std::to_underlying(button)));
    }

    auto InputState::mouse_position() const noexcept -> std::pair<i32, i32> {
        return { m_mouse_x, m_mouse_y };
    }

    auto InputState::mouse_delta() const noexcept -> std::pair<i32, i32> {
        return { m_mouse_dx, m_mouse_dy };
    }

    InputHandler::InputHandler(InputState& state, Events& events) noexcept
        : m_state{ state }, m_events{ events } {}

    auto InputHandler::handle_event(const SDL_Event& event) const noexcept -> void {
        switch(event.type) {
        case SDL_EVENT_KEY_UP: [[fallthrough]];
        case SDL_EVENT_KEY_DOWN:
            handle_key_event(event.key);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP: [[fallthrough]];
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            handle_mouse_button_event(event.button);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            handle_mouse_move_event(event.motion);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            handle_scroll_event(event.wheel);
            break;

        default:
            std::unreachable();
        }
    }

    auto InputHandler::handle_key_event(const SDL_KeyboardEvent& event) const noexcept -> void {
        const auto key = static_cast<Keys>(event.key);
        const auto action = static_cast<Actions>(event.down);
        const auto mods = static_cast<Modifiers>(event.mod);

        auto mod = event.mod;
        const auto scancode = SDL_GetScancodeFromKey(event.key, &mod);

        m_state.m_key_states.set(scancode, std::to_underlying(action));

        m_events.dispatch(KeyEvent{ .key = key, .action = action, .mods = mods });
    }

    auto InputHandler::handle_mouse_button_event(const SDL_MouseButtonEvent& event) const noexcept -> void {
        const auto button = static_cast<MouseButtons>(event.button);
        const auto action = static_cast<Actions>(event.down);

        m_state.m_mouse_button_states.set(std::to_underlying(button), std::to_underlying(action));

        m_events.dispatch(MouseButtonEvent{ .button = button, .action = action });
    }

    auto InputHandler::handle_mouse_move_event(const SDL_MouseMotionEvent& event) const noexcept -> void {
        m_state.m_mouse_x = static_cast<i32>(event.x);
        m_state.m_mouse_y = static_cast<i32>(event.y);

        m_state.m_mouse_dx = static_cast<i32>(event.xrel);
        m_state.m_mouse_dy = static_cast<i32>(event.yrel);

        m_events.dispatch(
            MouseMoveEvent{
                .x = m_state.m_mouse_x,
                .y = m_state.m_mouse_y,
                .dx = m_state.m_mouse_dx,
                .dy = m_state.m_mouse_dy,
            }
        );
    }

    auto InputHandler::handle_scroll_event(const SDL_MouseWheelEvent& event) const noexcept -> void {
        m_events.dispatch(
            ScrollEvent{
                .dx = event.integer_x,
                .dy = event.integer_y,
            }
        );
    }
}
