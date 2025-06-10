#include "Win32App.h"
#include "D3DApp.h"
#include "D3D12Engine.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
Win32App* Win32App::s_pInstance = nullptr;

Win32App::Win32App(HINSTANCE hInst, int nCmdShow, const wchar_t* title, int x, int y, int w, int h) 
	: m_bLMouseButtonDown(false), m_bRMouseButtonDown(false), m_iCurrMouseX(0), m_iCurrMouseY(0), m_iLastMouseX(0), m_iLastMouseY(0), m_fPhi(-90.0f), m_fTheta(0.0f), m_fRadius(2.0f)
{
	this->RegisterWin32Class(hInst);
	this->CreateWin32Window(title, x, y, w, h, nCmdShow);
	m_bRunning = false;
	s_pInstance = this;
}

HWND& Win32App::GetHWND()
{
	return hWnd;
}

Win32App* Win32App::GetWinApp()
{
	return s_pInstance;
}

void Win32App::SetLMouseDown(bool down)
{
	this->m_bLMouseButtonDown = down;
}

void Win32App::SetRMouseDown(bool down)
{
	this->m_bRMouseButtonDown = down;
}

bool Win32App::GetLMouseDown() const
{
	return m_bLMouseButtonDown;
}

bool Win32App::GetRMouseDown() const
{
	return m_bRMouseButtonDown;
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

	auto winApp = Win32App::GetWinApp();

	switch (uMsg)
	{
	case WM_KEYDOWN:	// When the Escape key is clicked
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(EXIT_SUCCESS);
			return 0;
			break;
		}
	case WM_MOUSEMOVE:

		if (winApp->GetLMouseDown())
		{
			winApp->m_iCurrMouseX = LOWORD(lParam);
			winApp->m_iCurrMouseY = HIWORD(lParam);


			int mouseDeltaX = winApp->m_iCurrMouseX - winApp->m_iLastMouseX;
			int mouseDeltaY = winApp->m_iCurrMouseY - winApp->m_iLastMouseY;

			winApp->m_iLastMouseX = winApp->m_iCurrMouseX;
			winApp->m_iLastMouseY = winApp->m_iCurrMouseY;

			winApp->m_fTheta -= (float)mouseDeltaY * 0.25f;
			winApp->m_fPhi -= (float)mouseDeltaX * 0.25f;

			winApp->m_fTheta = min(max(winApp->m_fTheta, -89.9f), 89.9f);

			XMFLOAT3 Eye;
			Eye.x = winApp->m_fRadius * cosf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));
			Eye.y = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fTheta));
			Eye.z = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));

			D3D12Engine::GetApp()->m_pCamera->SetLens(Eye);

			char buffer[70];
			sprintf_s(buffer, "mouse x: %f, y: %f\n", winApp->m_fTheta, winApp->m_fPhi);
			OutputDebugStringA(buffer);

		}
		if (winApp->GetRMouseDown())
		{
			winApp->m_iCurrMouseX = LOWORD(lParam);
			winApp->m_iCurrMouseY = HIWORD(lParam);

			int mouseDeltaX = winApp->m_iCurrMouseX - winApp->m_iLastMouseX;
			int mouseDeltaY = winApp->m_iCurrMouseY - winApp->m_iLastMouseY;

			winApp->m_iLastMouseX = winApp->m_iCurrMouseX;
			winApp->m_iLastMouseY = winApp->m_iCurrMouseY;

			winApp->m_fRadius += (float)mouseDeltaY * 0.02f;
			winApp->m_fRadius = min(max(winApp->m_fRadius, 0.1f), 10.0f);

			XMFLOAT3 Eye;
			Eye.x = winApp->m_fRadius * cosf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));
			Eye.y = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fTheta));
			Eye.z = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));

			D3D12Engine::GetApp()->m_pCamera->SetLens(Eye);

			char buffer[70];
			sprintf_s(buffer, "mouse x: %f, y: %f\n", winApp->m_fTheta, winApp->m_fPhi);
			OutputDebugStringA(buffer);
		}
		
		break;
	case WM_LBUTTONDOWN:
		winApp->SetLMouseDown(true);
		winApp->m_iLastMouseX = LOWORD(lParam);
		winApp->m_iLastMouseY = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		winApp->SetLMouseDown(false);
		break;
	case WM_RBUTTONDOWN:
		winApp->SetRMouseDown(true);
		winApp->m_iLastMouseX = LOWORD(lParam);
		winApp->m_iLastMouseY = HIWORD(lParam);
		break;
	case WM_RBUTTONUP:
		winApp->SetRMouseDown(false);
		break;
	case WM_DESTROY: // When the X button is clicked
		PostQuitMessage(EXIT_SUCCESS);		
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}