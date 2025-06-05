#include "Win32App.h"
#include "D3D12Engine.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    Win32App win32App(hInstance, nCmdShow, L"D3D12 App", 100, 100, 1024, 768);
    D3D12Engine d3dApp(win32App);

    while (!win32App.QuitMessagePosted())
    {
        win32App.DispatchMessages();

        d3dApp.OnUpdate();

        d3dApp.OnRender();
    }

    return 0;
}