// VizEngine/src/VizEngine/Events/ApplicationEvent.h

#pragma once

#include "Event.h"

namespace VizEngine
{
    class VizEngine_API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(int width, int height)
            : m_Width(width), m_Height(height) {}

        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        int m_Width, m_Height;
    };

    class VizEngine_API WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class VizEngine_API WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class VizEngine_API WindowLostFocusEvent : public Event
    {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowLostFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
