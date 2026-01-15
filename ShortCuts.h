#pragma once
#include <Qt>

namespace Shortcuts
{
    // ---- Mouse buttons ----
    constexpr Qt::MouseButton SelectButton = Qt::LeftButton;
    constexpr Qt::MouseButton PanButton = Qt::MiddleButton;

    // ---- Modifiers ----
    constexpr Qt::KeyboardModifier MultiSelectModifier = Qt::ShiftModifier;
    constexpr Qt::KeyboardModifier CreateModifier = Qt::ControlModifier;

    // ---- Keys ----
    constexpr Qt::Key SelectAllKey = Qt::Key_A;
    constexpr Qt::Key DeleteKey = Qt::Key_Backspace;
    constexpr Qt::Key FrameKey = Qt::Key_Escape;
}
