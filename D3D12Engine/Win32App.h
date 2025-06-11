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
	
	static Win32App* GetWinApp();

	bool GetLMouseDown() const;
	bool GetRMouseDown() const;
	void SetLMouseDown(bool down);
	void SetRMouseDown(bool down);

	int m_iCurrMouseX;
	int m_iCurrMouseY;

	int m_iLastMouseX;
	int m_iLastMouseY;

	float m_fPhi;
	float m_fTheta;
	float m_fRadius;

	bool m_bWireFrameEnabled;
private:
	static Win32App* s_pInstance;
	
	void RegisterWin32Class(HINSTANCE hInst);
	void CreateWin32Window(const wchar_t* title, int x, int y, int w, int h, int nCmdShow);

	bool m_bLMouseButtonDown;
	bool m_bRMouseButtonDown;

	bool m_bRunning;
	HINSTANCE hInstance;
	WNDCLASS wClass;
	HWND hWnd;
	MSG msg;
};

