#pragma once

#include "VizEngine/Core.h"
#include "glm.hpp"

// Forward declaration
struct GLFWwindow;

namespace VizEngine
{
	// Key codes - abstraction over GLFW key codes
	enum class KeyCode : int
	{
		// Printable keys
		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		
		D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,
		
		Semicolon = 59,
		Equal = 61,
		
		A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		GraveAccent = 96,
		
		// Function keys
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		
		F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		
		// Keypad
		KP0 = 320, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,
		
		// Modifiers
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	};

	// Mouse button codes
	enum class MouseCode : int
	{
		Button0 = 0,  // Left
		Button1 = 1,  // Right
		Button2 = 2,  // Middle
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,
		
		Left = Button0,
		Right = Button1,
		Middle = Button2
	};

	// Static input class for keyboard and mouse polling
	class VizEngine_API Input
	{
	public:
		// Initialize the input system (call once at startup)
		static void Init(GLFWwindow* window);
		
		// Update input state (call once per frame, before processing input)
		static void Update();
		
		// --- Keyboard ---
		
		// Returns true on the first frame the key is pressed
		static bool IsKeyPressed(KeyCode key);
		
		// Returns true while the key is held down
		static bool IsKeyHeld(KeyCode key);
		
		// Returns true on the frame the key is released
		static bool IsKeyReleased(KeyCode key);
		
		// --- Mouse Buttons ---
		
		// Returns true on the first frame the button is pressed
		static bool IsMouseButtonPressed(MouseCode button);
		
		// Returns true while the button is held down
		static bool IsMouseButtonHeld(MouseCode button);
		
		// Returns true on the frame the button is released
		static bool IsMouseButtonReleased(MouseCode button);
		
		// --- Mouse Position ---
		
		// Get current mouse position in window coordinates
		static glm::vec2 GetMousePosition();
		
		// Get mouse movement since last frame
		static glm::vec2 GetMouseDelta();
		
		// --- Scroll Wheel ---
		
		// Get scroll wheel delta since last frame (positive = up)
		static float GetScrollDelta();

		// GLFW callback (called by GLFWManager)
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		static GLFWwindow* s_Window;
		
		// Key state tracking for edge detection
		static bool s_CurrentKeys[512];
		static bool s_PreviousKeys[512];
		
		// Mouse button state tracking
		static bool s_CurrentMouseButtons[8];
		static bool s_PreviousMouseButtons[8];
		
		// Mouse position tracking
		static glm::vec2 s_MousePosition;
		static glm::vec2 s_LastMousePosition;
		static bool s_FirstMouse;
		
		// Scroll wheel
		static float s_ScrollDelta;
	};
}
