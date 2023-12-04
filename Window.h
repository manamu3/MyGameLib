#pragma once

#include <Windows.h>

#include <memory>
#include <functional>

class Window
{
private:
	std::unique_ptr<class Renderer> renderer;
	WNDCLASSEX windowClass;
	HWND hwnd;
	MSG msg;

	bool Show;

public:
	Window(int width, int height, double frameRate = 60.0);
	~Window();

public:
	bool Run(std::function<void()> process);
	class Renderer* GetRenderer() { return renderer.get(); }
	HWND GetHandle() { return hwnd; }

private:
	HWND WindowCreate(WNDCLASSEX& windowClass, int width, int height);
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

