#pragma once

#include "tool/DirectXTK2022/Keyboard.h"
#include "tool/DirectXTK2022/Mouse.h"
#include "tool/DirectXTK2022/Gamepad.h"

enum class MouseButton {
	LEFT_BUTTON,
	MIDDLE_BUTTON,
	RIGHT_BUTTON,
	X_BUTTON1,
	X_BUTTON2
};

enum class PadButton {
	A,
	B,
	X,
	Y,
	LEFT_STICK,
	RIGHT_STICK,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	VIEW,
	MENU,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT
};

class Input
{
private:
	std::unique_ptr<DirectX::Keyboard> keyboard;
	DirectX::Keyboard::KeyboardStateTracker keyTracker;

	std::unique_ptr<DirectX::Mouse> mouse;
	DirectX::Mouse::ButtonStateTracker mouseTracker;

	std::unique_ptr<DirectX::GamePad> gamepad;
	DirectX::GamePad::ButtonStateTracker gamepadTracker;

public:
	Input();
	void Update();

public:
	bool PressedKey(DirectX::Keyboard::Keys key);
	bool ReleasedKey(DirectX::Keyboard::Keys key);

	bool PressedMouse(MouseButton button);
	bool ReleasedMouse(MouseButton button);

	void SetVisibleCursor(bool visible);

	int GetCursorX();
	int GetCursorY();

	bool PressedButton(PadButton button);
	bool ReleasedButton(PadButton button);

	void SetVibration(int left, int right);

	float LeftStickX();
	float LeftStickY();
	float RightStickX();
	float RightStickY();
};