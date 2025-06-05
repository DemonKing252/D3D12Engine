#include "Win32App.h"
#include "D3DApp.h"
#include "D3D12Engine.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Win32App::Win32App(HINSTANCE hInst, int nCmdShow, const wchar_t* title, int x, int y, int w, int h)
{
	this->RegisterWin32Class(hInst);
	this->CreateWin32Window(title, x, y, w, h, nCmdShow);
	m_bRunning = false;
}

HWND& Win32App::GetHWND()
{
	return hWnd;
}

void Win32App::RegisterWin32Class(HINSTANCE hInst)
{
	ZeroMemory(&wClass, sizeof(WNDCLASS));
	
	// Default one for now
	hInstance = hInst;
	wClass.lpfnWndProc = WindowProc;
	wClass.hInstance = hInst;
	wClass.lpszClassName = L"win32class";

	RegisterClass(&wClass);
}

void Win32App::CreateWin32Window(const wchar_t* title, int x, int y, int w, int h, int nCmdShow)
{
	hWnd = CreateWindowEx(
		0,
		L"win32class",
		title,
		WS_OVERLAPPEDWINDOW,
		x,
		y,
		w,
		h,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hWnd, nCmdShow);

	msg = { 0 };
}


void Win32App::DispatchMessages()
{	
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			m_bRunning = true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



bool Win32App::QuitMessagePosted()
{
	return m_bRunning;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:	// When the Escape key is clicked
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(EXIT_SUCCESS);
			return 0;
		}
		break;
	case WM_DESTROY: // When the X button is clicked
		PostQuitMessage(EXIT_SUCCESS);		
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}