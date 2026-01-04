// VizEngine/src/VizEngine/Events/Event.h

#pragma once

#include <string>
#include <sstream>
#include <functional>
#include "VizEngine/Core.h"

namespace VizEngine
{
    // Event types
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // Event categories (bit flags for filtering)
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = (1 << 0),
        EventCategoryInput       = (1 << 1),
        EventCategoryKeyboard    = (1 << 2),
        EventCategoryMouse       = (1 << 3),
        EventCategoryMouseButton = (1 << 4)
    };

    // Abstract base class for all events
    class VizEngine_API Event
    {
    public:
        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) const
        {
            return GetCategoryFlags() & category;
        }

        bool Handled = false;
    };

    // Type-safe event dispatcher
    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    using EventCallbackFn = std::function<void(Event&)>;
}

// Macros to reduce boilerplate in event classes
#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }
