#pragma once
#include <Windows.h>
class Win32App
{
public:
	Win32App(Win32App& rhs) = delete;	// Delete the copy constructor	
	Win32App(HINSTANCE hInst, int nCmdShow, const wchar_t* title, int x, int y, int w, int h);

	void DispatchMessages();
	bool QuitMessagePosted();

	HWND& GetHWND();
private:

	void RegisterWin32Class(HINSTANCE hInst);
	void CreateWin32Window(const wchar_t* title, int x, int y, int w, int h, int nCmdShow);

	bool m_bRunning;
	HINSTANCE hInstance;
	WNDCLASS wClass;
	HWND hWnd;
	MSG msg;
};

