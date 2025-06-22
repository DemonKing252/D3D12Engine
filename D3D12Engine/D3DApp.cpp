#include "D3DApp.h"
#include "Win32App.h"

D3DApp::D3DApp(Win32App& win32App)
{
	this->WinApp = &win32App;
    m_fenceValue = 0;
}

ID3D12Device* D3DApp::GetDev() const
{
    return m_device.Get();
}

void D3DApp::InitializeD3D()
{
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
    m_debugController->EnableDebugLayer();

    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_device)));

    ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    
    ThrowIfFailed(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)));

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    ThrowIfFailed(m_device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    ThrowIfFailed(m_commandList->Close());

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

    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_handle = CreateEvent(0, 0, 0, 0);

    this->BuildRenderTarget();
    this->BuildDepthStencilViews();
}

void D3DApp::BuildRenderTarget()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    rtvHeapDesc.NumDescriptors = m_iNumBuffers;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
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

void D3DApp::BuildDepthStencilViews()
{
    // step 1: create 2d texture that will store the depth
    D3D12_RESOURCE_DESC depthStencilTextureDesc;
    ZeroMemory(&depthStencilTextureDesc, sizeof(D3D12_RESOURCE_DESC));

    depthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.DepthOrArraySize = 1;
    depthStencilTextureDesc.Width = 1024;
    depthStencilTextureDesc.Height = 768;
    depthStencilTextureDesc.SampleDesc.Count = 1;
    depthStencilTextureDesc.SampleDesc.Quality = 0;
    depthStencilTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue;
    ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));

    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0.0f;

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilTextureDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(&m_pDSVResource)
    ));

    // step 2: create depth stencil view descriptor
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));

    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // step 3: create a descriptor heap to hold our resource
    D3D12_DESCRIPTOR_HEAP_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    depthStencilDesc.NumDescriptors = 1;
    depthStencilDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(m_device->CreateDescriptorHeap(&depthStencilDesc, IID_PPV_ARGS(&m_pDSVDescriptorHeap)));

    // step 4: create depth stencil view
    m_device->CreateDepthStencilView(m_pDSVResource.Get(), &depthStencilViewDesc, m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    ZeroMemory(&m_depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

    // step 5: depth comparison flags for the pipeline state
    m_depthStencilDesc.DepthEnable = true;
    m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
}

void D3DApp::WaitForGPU()
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

void D3DApp::ExecuteCommandList()
{
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(1, cmdLists);
}
