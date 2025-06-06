#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include <string>
#include <comdef.h>
#include <stdexcept>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include <d3dcompiler.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
using Microsoft::WRL::ComPtr;

inline std::string HResultToString(HRESULT hr)
{
	_com_error err(hr);
	const wchar_t* wideMsg = err.ErrorMessage();

	// Convert to std::string (narrow) from wide
	char narrowMsg[512];
	size_t converted = 0;
	wcstombs_s(&converted, narrowMsg, wideMsg, _TRUNCATE);

	return std::string(narrowMsg);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(hrCall)                                      \
    {                                                              \
        HRESULT _hr = (hrCall);                                    \
        if (FAILED(_hr))                                           \
        {                                                          \
            throw std::runtime_error(HResultToString(_hr).c_str());\
        }                                                          \
    }
#endif


class Win32App;
class D3DApp
{
private:
	Win32App* WinApp;
public:
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	
	void Sync();;
	void PresentFrame();

	D3DApp(Win32App& win32App);
protected:
	UINT64 m_fenceValue;
	const UINT32 m_iNumBuffers = 3;

	ComPtr<IDXGISwapChain3> m_swapChain3;
	ComPtr<ID3D12Fence> m_fence;
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIFactory2> m_factory;
	ComPtr<ID3D12Debug> m_debugController;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<IDXGISwapChain1> m_swapChain;
	ComPtr<ID3D12Resource> m_rtvResources[3];
	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
	HANDLE m_handle;

	void InitializeD3D();
	void BuildRenderTarget();
};