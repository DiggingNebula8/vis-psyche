#pragma once

#include"Event.h"

#include<sstream>

namespace VizEngine
{
	class VizEngine_API MouseMoved : public Event
	{
	public:
		MouseMoved(float x, float y)
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

	class VizEngine_API MouseScrolled : public Event
	{
	public:
		MouseScrolled(float xOffset, float yOffset)
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

	class VizEngine_API MouseButton : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	protected:
		MouseButton(int button)
			: m_Button(button) {}
		int m_Button;
	};

	class VizEngine_API MouseButtonPressed : public MouseButton
	{
	public:
		MouseButtonPressed(int button)
			: MouseButton(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button pressed: " << m_Button;
			return ss.str();
		}


		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class VizEngine_API MouseButtonReleased : public MouseButton
	{
	public:
		MouseButtonReleased(int button)
			: MouseButton(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button released: " << m_Button;
			return ss.str();
		}


		EVENT_CLASS_TYPE(MouseButtonReleased)
	};



}