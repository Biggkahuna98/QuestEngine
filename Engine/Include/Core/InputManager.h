#pragma once

#include "InputCodes.h"
#include "glm/glm.hpp"
#include <map>
#include <memory>
#include <string>

namespace QE
{
	struct KeyData
	{
		KeyCode Key;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	struct MouseButtonData
	{
		MouseCode Button;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	class InputManager
	{
	public:
		InputManager(const std::string_view windowName);
		~InputManager();

		bool IsKeyPressed(KeyCode key);
		bool IsKeyHeld(KeyCode key);
		bool IsKeyDown(KeyCode key);
		bool IsKeyReleased(KeyCode key);

		bool IsMouseButtonPressed(MouseCode button);
		bool IsMouseButtonHeld(MouseCode button);
		bool IsMouseButtonDown(MouseCode button);
		bool IsMouseButtonReleased(MouseCode button);
		glm::vec2 GetMousePosition();
		double GetMouseX();
		double GetMouseY();

		void ProcessTransitions();
		void UpdateKeyState(KeyCode key, KeyState newState);
		void UpdateMouseButtonState(MouseCode mouse, KeyState newState);
		void UpdateMousePosition(double x, double y);
		void UpdatePressedKeysToHeld();
		void UpdatePressedMouseButtonsToHeld();
		void ClearReleasedKeys();

		void SetWindowName(const std::string_view windowName);
	private:
		std::string m_WindowName; // window name associated with this input manager
		std::map<KeyCode, KeyData> m_KeyData;
		std::map<MouseCode, MouseButtonData> m_MouseData;
		double m_MouseXPosition = 0.0f;
		double m_MouseYPosition = 0.0f;
	};

}