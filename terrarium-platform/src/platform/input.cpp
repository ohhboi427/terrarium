#include <platform/input.hpp>
#include <terrarium/platform/input.hpp>

#include <terrarium/core/app.hpp>

namespace terra::platform {
    using namespace core;

    Modifiers::Modifiers(const Keys keys) noexcept
        : m_keys{ keys } {}

    auto Modifiers::has_all(const Keys keys) const noexcept -> bool {
        return (m_keys & keys) == keys;
    }

    auto Modifiers::has_any(const Keys keys) const noexcept -> bool {
        return (m_keys & keys) != None;
    }

    auto InputState::handle_event(const SDL_Event& event, Events& events) noexcept -> void {
        switch(event.type) {
        case SDL_EVENT_KEY_UP: [[fallthrough]];
        case SDL_EVENT_KEY_DOWN:
            handle_key_event(event.key, events);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP: [[fallthrough]];
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            handle_mouse_button_event(event.button, events);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            handle_mouse_move_event(event.motion, events);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            handle_scroll_event(event.wheel, events);
            break;

        default:
            std::unreachable();
        }
    }

    auto InputState::handle_key_event(const SDL_KeyboardEvent& event, Events& events) noexcept -> void {
        const auto key = static_cast<Keys>(event.key);
        const auto action = static_cast<Actions>(event.down);
        const Modifiers mods{ static_cast<Modifiers::Keys>(event.mod) };

        const auto scancode = SDL_GetScancodeFromKey(event.key, nullptr);
        key_states.set(scancode, std::to_underlying(action));

        events.dispatch(KeyEvent{ .key = key, .action = action, .mods = mods });
    }

    auto InputState::handle_mouse_button_event(const SDL_MouseButtonEvent& event, Events& events) noexcept -> void {
        const auto button = static_cast<MouseButtons>(event.button);
        const auto action = static_cast<Actions>(event.down);

        mouse_button_states.set(std::to_underlying(button), std::to_underlying(action));

        events.dispatch(MouseButtonEvent{ .button = button, .action = action });
    }

    auto InputState::handle_mouse_move_event(const SDL_MouseMotionEvent& event, Events& events) noexcept -> void {
        mouse_position = {
            static_cast<i32>(event.x),
            static_cast<i32>(event.y),
        };

        mouse_delta = {
            static_cast<i32>(event.xrel),
            static_cast<i32>(event.yrel),
        };

        events.dispatch(
            MouseMoveEvent{
                .position = mouse_position,
                .delta = mouse_delta,
            }
        );
    }

    auto InputState::handle_scroll_event(const SDL_MouseWheelEvent& event, Events& events) noexcept -> void { // NOLINT
        events.dispatch(
            ScrollEvent{
                .dx = event.integer_x,
                .dy = event.integer_y,
            }
        );
    }

    Inputs::Inputs(const App& app) noexcept
        : m_internal{ app.get_service<InputState>() } {}

    auto Inputs::key_state(const Keys key) const noexcept -> Actions {
        const auto scancode = SDL_GetScancodeFromKey(std::to_underlying(key), nullptr);
        return static_cast<Actions>(m_internal.key_states.test(scancode));
    }

    auto Inputs::mouse_button_state(const MouseButtons button) const noexcept -> Actions {
        return static_cast<Actions>(m_internal.mouse_button_states.test(std::to_underlying(button)));
    }

    auto Inputs::mouse_position() const noexcept -> std::pair<i32, i32> {
        return m_internal.mouse_position;
    }

    auto Inputs::mouse_delta() const noexcept -> std::pair<i32, i32> {
        return m_internal.mouse_delta;
    }
}

namespace terra::core {
    using namespace platform;

    auto IExtractor<Inputs>::operator()(const World&, const App& app) noexcept -> Inputs {
        return Inputs{ app };
    }
}
