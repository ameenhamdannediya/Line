#pragma once
#include <Qt>
#include <QKeySequence>

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

    // ---- Menu shortcuts (QKeySequence = NOT constexpr) ----
    inline const QKeySequence Save = QKeySequence(Qt::CTRL | Qt::Key_S);
    inline const QKeySequence SaveAs = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    inline const QKeySequence Open = QKeySequence(Qt::CTRL | Qt::Key_O);
    inline const QKeySequence NewFile = QKeySequence(Qt::CTRL | Qt::Key_N);
    inline const QKeySequence QuitSeq = QKeySequence(Qt::CTRL | Qt::Key_Q);
}
