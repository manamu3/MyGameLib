#include "Window.h"

#include "Renderer.h"
#include "FPS.h"
#include <iostream>
#include "Keyboard.h"
#include "Mouse.h"

Window::Window(int width, int height, double frameRate) {
	windowClass = {};
	hwnd = WindowCreate(windowClass, width, height);

	renderer = std::make_unique<Renderer>(width, height, hwnd);

	FPS::Initialize(frameRate);

	Show = false;

	msg = {};
}

Window::~Window() {
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool Window::Run(std::function<void()> process) {
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		return false;
	}

	FPS::Run([&] {
		renderer->BeginDraw();

		process();

		renderer->EndDraw();

		if (!Show) {
			ShowWindow(hwnd, SW_SHOW);
			UpdateWindow(hwnd);
			Show = true;
		}

		return true;
	});

	return true;
}

HWND Window::WindowCreate(WNDCLASSEX& windowClass, int width, int height) {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProc;
	windowClass.lpszClassName = TEXT("GameEngine");
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, width, height };

	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);
	windowRect.right -= windowRect.left;
	windowRect.bottom -= windowRect.top;
	windowRect.left = 0;
	windowRect.top = 0;

	HWND hwnd = CreateWindow(windowClass.lpszClassName, TEXT("ウィンドウ"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right, windowRect.bottom,
		nullptr, nullptr, windowClass.hInstance, nullptr);

	return hwnd;
}

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	switch (msg)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_SYSKEYDOWN:
		if (wparam == VK_RETURN && (lparam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
			
		}
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);
	}

	switch (msg)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_MOUSEACTIVATE:
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}