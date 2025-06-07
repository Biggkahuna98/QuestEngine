#include "Core/InputManager.h"
#include "Core/Log.h"

namespace QE
{
	InputManager::InputManager(const std::string_view windowName)
		: m_WindowName(windowName.data())
	{
	}

	InputManager::~InputManager()
	{
	}

	bool InputManager::IsKeyPressed(KeyCode key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Pressed;
	}

	bool InputManager::IsKeyHeld(KeyCode key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Held;
	}

	bool InputManager::IsKeyDown(KeyCode key)
	{
		return IsKeyPressed(key) || IsKeyHeld(key);
	}

	bool InputManager::IsKeyReleased(KeyCode key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Released;
	}

	bool InputManager::IsMouseButtonPressed(MouseCode button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Pressed;
	}

	bool InputManager::IsMouseButtonHeld(MouseCode button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Held;
	}

	bool InputManager::IsMouseButtonDown(MouseCode button)
	{
		return IsMouseButtonPressed(button) || IsMouseButtonHeld(button);
	}

	bool InputManager::IsMouseButtonReleased(MouseCode button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Released;
	}

	glm::vec2 InputManager::GetMousePosition()
	{
		return { m_MouseXPosition, m_MouseYPosition };
	}

	double InputManager::GetMouseX()
	{
		return m_MouseXPosition;
	}

	double InputManager::GetMouseY()
	{
		return m_MouseYPosition;
	}

	glm::vec2 InputManager::GetMouseOffsets()
	{
		return { m_MouseXOffset, m_MouseYOffset };
	}

	void InputManager::ProcessTransitions()
	{
		UpdatePressedKeysToHeld();
		UpdatePressedMouseButtonsToHeld();
	}

	void InputManager::UpdateKeyState(KeyCode key, KeyState newState)
	{
		auto& keyData = m_KeyData[key];
		keyData.OldState = keyData.State;
		keyData.State = newState;
		//LOG_DEBUG_TAG("Input", "[{}]: {} is {}", m_WindowName, static_cast<char>(key), GetKeyStateString(newState));
	}

	void InputManager::UpdateMouseButtonState(MouseCode mouse, KeyState newState)
	{
		auto& mouseData = m_MouseData[mouse];
		mouseData.OldState = mouseData.State;
		mouseData.State = newState;
		//LOG_DEBUG_TAG("Input", "[{}]: {} is {}", m_WindowName, GetMouseButtonStringFromCode(mouse), GetKeyStateString(newState));
	}

	void InputManager::UpdateMousePosition(double x, double y)
	{
		if (FirstMouse)
		{
			m_MouseXPosition = x;
			m_MouseYPosition = y;
			FirstMouse = false;
		}

		m_MouseXOffset = x - m_MouseXPosition;
		m_MouseYOffset = m_MouseYPosition - y;

		m_MouseXPosition = x;
		m_MouseYPosition = y;
	}

	void InputManager::UpdatePressedKeysToHeld()
	{
		for (const auto& [key, keyData] : m_KeyData)
		{
			if (keyData.State == KeyState::Pressed)
				UpdateKeyState(key, KeyState::Held);
		}
	}

	void InputManager::UpdatePressedMouseButtonsToHeld()
	{
		for (const auto& [button, buttonData] : m_MouseData)
		{
			if (buttonData.State == KeyState::Pressed)
				UpdateMouseButtonState(button, KeyState::Held);
		}
	}

	void InputManager::ClearReleasedKeys()
	{
		for (const auto& [key, keyData] : m_KeyData)
		{
			if (keyData.State == KeyState::Released)
				UpdateKeyState(key, KeyState::None);
		}

		for (const auto& [button, buttonData] : m_MouseData)
		{
			if (buttonData.State == KeyState::Released)
				UpdateMouseButtonState(button, KeyState::None);
		}
	}

	void InputManager::SetWindowName(const std::string_view windowName)
	{
		m_WindowName = windowName.data();
	}
}