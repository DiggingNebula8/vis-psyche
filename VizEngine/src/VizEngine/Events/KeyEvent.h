// VizEngine/src/VizEngine/Events/KeyEvent.h

#pragma once

#include "Event.h"
#include "VizEngine/Core/Input.h"

namespace VizEngine
{
    class VizEngine_API KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode m_KeyCode;
    };

    class VizEngine_API KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode keycode, bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode)
               << (m_IsRepeat ? " (repeat)" : "");
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        bool m_IsRepeat;
    };

    class VizEngine_API KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class VizEngine_API KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
