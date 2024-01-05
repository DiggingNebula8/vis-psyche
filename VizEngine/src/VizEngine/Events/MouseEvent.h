#pragma once

#include"Event.h"

#include<sstream>

namespace VizEngine
{
	class VizEngine_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse moved: " << GetX() << "," << GetY();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	private:
		float m_MouseX, m_MouseY;

	};

	class VizEngine_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			:m_MouseXOffset(xOffset), m_MouseYOffset(yOffset) {}

		inline float GetXOffset() const { return m_MouseXOffset; }
		inline float GetYOffset() const { return m_MouseYOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse scrolled: " << GetXOffset() << "," << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	private:
		float m_MouseXOffset, m_MouseYOffset;
	};

	class VizEngine_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}
		int m_Button;
	};

	class VizEngine_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button pressed: " << m_Button;
			return ss.str();
		}


		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class VizEngine_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button released: " << m_Button;
			return ss.str();
		}


		EVENT_CLASS_TYPE(MouseButtonReleased)
	};



}