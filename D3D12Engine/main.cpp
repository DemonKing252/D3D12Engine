#include "Win32App.h"
#include "D3D12Engine.h"
/*
    Project:       D3D12Engine
    Author:         Liam Blake
    Created:        2025-05-28
    Last Modified:  2025-07-16
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    Win32App win32App(hInstance, nCmdShow, L"D3D12 Engine", 100, 100, 1024, 768);
    D3D12Engine::Init(win32App);

    while (!win32App.QuitMessagePosted())
    {
        win32App.DispatchMessages();

        D3D12Engine::Instance()->ResetCommandObjects();

        D3D12Engine::Instance()->OnUpdate();
        D3D12Engine::Instance()->OnRender();
        
        D3D12Engine::Instance()->ExecuteCommandList();
        D3D12Engine::Instance()->PresentFrame();
        D3D12Engine::Instance()->WaitForGPU();
    }

    return 0;
}