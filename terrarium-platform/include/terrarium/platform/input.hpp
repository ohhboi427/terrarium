#pragma once

#include <terrarium/core/event.hpp>
#include <terrarium/platform/base.hpp>

#include <utility>

namespace terra::platform {
    enum class Keys : u32 {
        None = 0U,

        A = 0x00000061U,
        B = 0x00000062U,
        C = 0x00000063U,
        D = 0x00000064U,
        E = 0x00000065U,
        F = 0x00000066U,
        G = 0x00000067U,
        H = 0x00000068U,
        I = 0x00000069U,
        J = 0x0000006AU,
        K = 0x0000006BU,
        L = 0x0000006CU,
        M = 0x0000006DU,
        N = 0x0000006EU,
        O = 0x0000006FU,
        P = 0x00000070U,
        Q = 0x00000071U,
        R = 0x00000072U,
        S = 0x00000073U,
        T = 0x00000074U,
        U = 0x00000075U,
        V = 0x00000076U,
        W = 0x00000077U,
        X = 0x00000078U,
        Y = 0x00000079U,
        Z = 0x0000007AU,

        Alpha0 = 0x00000030U,
        Alpha1 = 0x00000031U,
        Alpha2 = 0x00000032U,
        Alpha3 = 0x00000033U,
        Alpha4 = 0x00000034U,
        Alpha5 = 0x00000035U,
        Alpha6 = 0x00000036U,
        Alpha7 = 0x00000037U,
        Alpha8 = 0x00000038U,
        Alpha9 = 0x00000039U,

        Return = 0x0000000DU,
        Escape = 0x0000001BU,
        Backspace = 0x00000008U,
        Tab = 0x00000009U,
        Space = 0x00000020U,
        Delete = 0x0000007FU,
        CapsLock = 0x40000039U,
        Up = 0x40000052U,
        Down = 0x40000051U,
        Left = 0x40000050U,
        Right = 0x4000004FU,
        PrintScreen = 0x40000046U,
        ScrollLock = 0x40000047U,
        Pause = 0x40000048U,
        Insert = 0x40000049U,
        Home = 0x4000004AU,
        End = 0x4000004DU,
        PageUp = 0x4000004BU,
        PageDown = 0x4000004EU,
        Application = 0x40000065U,
        Power = 0x40000066U,

        LeftControl = 0x400000E0U,
        LeftShift = 0x400000E1U,
        LeftAlt = 0x400000E2U,
        LeftSuper = 0x400000E3U,
        RightControl = 0x400000E4U,
        RightShift = 0x400000E5U,
        RightAlt = 0x400000E6U,
        RightSuper = 0x400000E7U,

        F1 = 0x4000003AU,
        F2 = 0x4000003BU,
        F3 = 0x4000003CU,
        F4 = 0x4000003DU,
        F5 = 0x4000003EU,
        F6 = 0x4000003FU,
        F7 = 0x40000040U,
        F8 = 0x40000041U,
        F9 = 0x40000042U,
        F10 = 0x40000043U,
        F11 = 0x40000044U,
        F12 = 0x40000045U,
        F13 = 0x40000068U,
        F14 = 0x40000069U,
        F15 = 0x4000006AU,
        F16 = 0x4000006BU,
        F17 = 0x4000006CU,
        F18 = 0x4000006DU,
        F19 = 0x4000006EU,
        F20 = 0x4000006FU,
        F21 = 0x40000070U,
        F22 = 0x40000071U,
        F23 = 0x40000072U,
        F24 = 0x40000073U,

        Numpad0 = 0x40000062U,
        Numpad1 = 0x40000059U,
        Numpad2 = 0x4000005AU,
        Numpad3 = 0x4000005BU,
        Numpad4 = 0x4000005CU,
        Numpad5 = 0x4000005DU,
        Numpad6 = 0x4000005EU,
        Numpad7 = 0x4000005FU,
        Numpad8 = 0x40000060U,
        Numpad9 = 0x40000061U,
        NumpadPeriod = 0x40000063U,
        NumpadPlus = 0x40000057U,
        NumpadMinus = 0x40000056U,
        NumpadMultiply = 0x40000055U,
        NumpadDivide = 0x40000054U,
        NumpadEnter = 0x40000058U,
        NumpadEquals = 0x40000067U,
        NumLock = 0x40000053U,

        Exclaim = 0x00000021U,
        DoubleQuote = 0x00000022U,
        Hash = 0x00000023U,
        Dollar = 0x00000024U,
        Percent = 0x00000025U,
        Ampersand = 0x00000026U,
        Quote = 0x00000027U,
        LeftParen = 0x00000028U,
        RightParen = 0x00000029U,
        Asterisk = 0x0000002AU,
        Plus = 0x0000002BU,
        Comma = 0x0000002CU,
        Minus = 0x0000002DU,
        Period = 0x0000002EU,
        Slash = 0x0000002FU,
        Colon = 0x0000003AU,
        Semicolon = 0x0000003BU,
        Less = 0x0000003CU,
        Equals = 0x0000003DU,
        Greater = 0x0000003EU,
        Question = 0x0000003FU,
        At = 0x00000040U,
        LeftBracket = 0x0000005BU,
        Backslash = 0x0000005CU,
        RightBracket = 0x0000005DU,
        Caret = 0x0000005EU,
        Underscore = 0x0000005FU,
        Backquote = 0x00000060U,
        LeftBrace = 0x0000007BU,
        Pipe = 0x0000007CU,
        RightBrace = 0x0000007DU,
        Tilde = 0x0000007EU,
        PlusMinus = 0x000000B1U,

        Mute = 0x4000007FU,
        VolumeUp = 0x40000080U,
        VolumeDown = 0x40000081U,
        MediaPlay = 0x40000106U,
        MediaPause = 0x40000107U,
        MediaRecord = 0x40000108U,
        MediaFastForward = 0x40000109U,
        MediaRewind = 0x4000010AU,
        MediaNextTrack = 0x4000010BU,
        MediaPreviousTrack = 0x4000010CU,
        MediaStop = 0x4000010DU,
        MediaEject = 0x4000010EU,
        MediaPlayPause = 0x4000010FU,
        MediaSelect = 0x40000110U,
    };

    enum class MouseButtons : u8 {
        None = 0U,

        Left = 1U,
        Middle = 2U,
        Right = 3U,
        Back = 4U,
        Forward = 5U,
    };

    enum class Modifiers : u16 {
        None = 0U,

        LeftShift = 0x0001U,
        RightShift = 0x0002U,
        Level5 = 0x0004U,
        LeftCtrl = 0x0040U,
        RightCtrl = 0x0080U,
        LeftAlt = 0x0100U,
        RightAlt = 0x0200U,
        LeftSuper = 0x0400U,
        RightSuper = 0x0800U,
        NumLock = 0x1000U,
        CapsLock = 0x2000U,
        Mode = 0x4000U,
        ScrollLock = 0x8000U,
        Ctrl = LeftCtrl | RightCtrl,
        Shift = LeftShift | RightShift,
        Alt = LeftAlt | RightAlt,
        Super = LeftSuper | RightSuper,
    };

    [[nodiscard]] constexpr auto operator|(const Modifiers lhs, const Modifiers rhs) noexcept -> Modifiers {
        return static_cast<Modifiers>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    [[nodiscard]] constexpr auto operator&(const Modifiers lhs, const Modifiers rhs) noexcept -> Modifiers {
        return static_cast<Modifiers>(std::to_underlying(lhs) & std::to_underlying(rhs));
    }

    enum class Actions : bool {
        Press = true,
        Release = false,
    };

    struct TERRA_PLATFORM_API KeyEvent : core::IEvent {
        Keys key;
        Actions action;
        Modifiers mods;
    };

    struct TERRA_PLATFORM_API MouseButtonEvent : core::IEvent {
        MouseButtons button;
        Actions action;
    };

    struct TERRA_PLATFORM_API MouseMoveEvent : core::IEvent {
        i32 x;
        i32 y;
        i32 dx;
        i32 dy;
    };

    struct TERRA_PLATFORM_API ScrollEvent : core::IEvent {
        i32 dx;
        i32 dy;
    };
}
