#include "Input.h"

Input::Input() {
	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();
	gamepad = std::make_unique<DirectX::GamePad>();
}

void Input::Update() {
	DirectX::Keyboard::State keyState = keyboard->GetState();
	keyTracker.Update(keyState);

	DirectX::Mouse::State mouseState = mouse->GetState();
	mouseTracker.Update(mouseState);

	DirectX::GamePad::State gamepadState = gamepad->GetState(0, DirectX::GamePad::DEAD_ZONE_NONE);
	if (gamepadState.IsConnected())
	{
		gamepadTracker.Update(gamepadState);
	}
	else {
		gamepadTracker.Reset();
	}
}

bool Input::PressedKey(DirectX::Keyboard::Keys key) {
	return keyTracker.IsKeyPressed(key);
}

bool Input::ReleasedKey(DirectX::Keyboard::Keys key) {
	return keyTracker.IsKeyReleased(key);
}

bool Input::PressedMouse(MouseButton button) {
	if (button == MouseButton::LEFT_BUTTON) {
		return mouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED;
	}
	else if (button == MouseButton::MIDDLE_BUTTON) {
		return mouseTracker.middleButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED;
	}
	else if (button == MouseButton::RIGHT_BUTTON) {
		return mouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED;
	}
	else if (button == MouseButton::X_BUTTON1) {
		return mouseTracker.xButton1 == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED;
	}
	else if (button == MouseButton::X_BUTTON2) {
		return mouseTracker.xButton2 == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED;
	}

	return false;
}

bool Input::ReleasedMouse(MouseButton button) {
	if (button == MouseButton::LEFT_BUTTON) {
		return mouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED;
	}
	else if (button == MouseButton::MIDDLE_BUTTON) {
		return mouseTracker.middleButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED;
	}
	else if (button == MouseButton::RIGHT_BUTTON) {
		return mouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED;
	}
	else if (button == MouseButton::X_BUTTON1) {
		return mouseTracker.xButton1 == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED;
	}
	else if (button == MouseButton::X_BUTTON2) {
		return mouseTracker.xButton2 == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED;
	}

	return false;
}

void Input::SetVisibleCursor(bool visible) {
	mouse->SetVisible(visible);
}

int Input::GetCursorX() {
	return mouse->GetState().x;
}

int Input::GetCursorY() {
	return mouse->GetState().y;
}

bool Input::PressedButton(PadButton button) {
	switch (button) {
	case PadButton::A:
		return gamepadTracker.a == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::B:
		return gamepadTracker.b == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::X:
		return gamepadTracker.x == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::Y:
		return gamepadTracker.y == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::LEFT_STICK:
		return gamepadTracker.leftStick == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::RIGHT_STICK:
		return gamepadTracker.rightStick == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::LEFT_SHOULDER:
		return gamepadTracker.leftShoulder == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::RIGHT_SHOULDER:
		return gamepadTracker.rightShoulder == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::VIEW:
		return gamepadTracker.view == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::MENU:
		return gamepadTracker.menu == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::DPAD_UP:
		return gamepadTracker.dpadUp == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::DPAD_DOWN:
		return gamepadTracker.dpadDown == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::DPAD_LEFT:
		return gamepadTracker.dpadLeft == DirectX::GamePad::ButtonStateTracker::PRESSED;
	case PadButton::DPAD_RIGHT:
		return gamepadTracker.dpadRight == DirectX::GamePad::ButtonStateTracker::PRESSED;
	}

	return false;
}

bool Input::ReleasedButton(PadButton button) {
	switch (button) {
	case PadButton::A:
		return gamepadTracker.a == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::B:
		return gamepadTracker.b == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::X:
		return gamepadTracker.x == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::Y:
		return gamepadTracker.y == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::LEFT_STICK:
		return gamepadTracker.leftStick == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::RIGHT_STICK:
		return gamepadTracker.rightStick == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::LEFT_SHOULDER:
		return gamepadTracker.leftShoulder == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::RIGHT_SHOULDER:
		return gamepadTracker.rightShoulder == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::VIEW:
		return gamepadTracker.view == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::MENU:
		return gamepadTracker.menu == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::DPAD_UP:
		return gamepadTracker.dpadUp == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::DPAD_DOWN:
		return gamepadTracker.dpadDown == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::DPAD_LEFT:
		return gamepadTracker.dpadLeft == DirectX::GamePad::ButtonStateTracker::RELEASED;
	case PadButton::DPAD_RIGHT:
		return gamepadTracker.dpadRight == DirectX::GamePad::ButtonStateTracker::RELEASED;
	}

	return false;
}

void Input::SetVibration(int left, int right) {
	gamepad->SetVibration(0, left, right);
}

float Input::LeftStickX() {
	return gamepad->GetState(0).thumbSticks.leftX;
}

float Input::LeftStickY() {
	return gamepad->GetState(0).thumbSticks.leftY;
}

float Input::RightStickX() {
	return gamepad->GetState(0).thumbSticks.rightX;
}

float Input::RightStickY() {
	return gamepad->GetState(0).thumbSticks.rightY;
}