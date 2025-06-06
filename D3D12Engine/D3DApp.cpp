#include "D3DApp.h"
#include "Win32App.h"

D3DApp::D3DApp(Win32App& win32App)
{
	this->WinApp = &win32App;
    m_fenceValue = 0;
}

void D3DApp::InitializeD3D()
{
    // step 1: enable debug controller
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
    m_debugController->EnableDebugLayer();

    // step 2: create the device
    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_device)));

    // step 3: create the factory
    ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));

    // step 4: create command queue
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    
    ThrowIfFailed(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // step 5: create command allocator
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    // step 6: create command list
    ThrowIfFailed(m_device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    ThrowIfFailed(m_commandList->Close());

    // step 7: create swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

    swapChainDesc.BufferCount = 3; // Number of back buffers
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Width = 1024;
    swapChainDesc.Height = 768;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        WinApp->GetHWND(),
        &swapChainDesc,
        nullptr,
        nullptr,
        m_swapChain.GetAddressOf()
    ));

    // step 8: create fence
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_handle = CreateEvent(0, 0, 0, 0);

}

void D3DApp::BuildRenderTarget()
{
    // step 9: create a descriptor heap which will hold the back buffer memory addresses
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    rtvHeapDesc.NumDescriptors = m_iNumBuffers;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    // step 10: create render target views and store them in the heap
    for (UINT rtvIndex = 0; rtvIndex < m_iNumBuffers; rtvIndex++)
    {
        // get back buffer resource
        ThrowIfFailed(m_swapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&m_rtvResources[rtvIndex])));

        // create render target view
        m_device->CreateRenderTargetView(m_rtvResources[rtvIndex].Get(), nullptr, rtvHandle);

        // offset the descriptor handle
        rtvHandle.Offset(1, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
    }
}

void D3DApp::Sync()
{
    // Wait for GPU to catch up to the commands being recorded in the command queue
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_handle));
        WaitForSingleObject(m_handle, INFINITE);
    }
    m_fenceValue++;
}

void D3DApp::PresentFrame()
{
    // Present(1, 0) to keep V Sync on. Or Present(0, 0) to keep V Sync off
    ThrowIfFailed(m_swapChain->Present(1, 0));
}