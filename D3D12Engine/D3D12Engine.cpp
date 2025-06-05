#include "D3D12Engine.h"
#include "Win32App.h"

D3D12Engine::D3D12Engine(Win32App& win32App) : D3DApp(win32App)
{
	this->D3DApp::InitializeD3D();
	this->D3DApp::BuildDescriptorHeaps();
	this->CompileShaders();
	this->CreateGraphicsPipeline();
	this->CreateBuffers();
	
}

void D3D12Engine::CreateBuffers()
{

	MeshGeometry triangleMesh = GeometryGenerator::CreateTriangle();

	const UINT sizeInBytes = sizeof(Vertex) * triangleMesh.m_vVertices.size();

	m_pVertexBuffer = std::make_unique<UploadBuffer<Vertex>>(m_device.Get(), triangleMesh.m_vVertices.data(), sizeInBytes);

	ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = sizeInBytes;

	for (const auto& v : triangleMesh.m_vVertices)
	{
		char buffer[128];
		sprintf_s(buffer, "Vertex Pos: (%f, %f, %f), Color: (%f, %f, %f)\n",
			v.Position.x, v.Position.y, v.Position.z,
			v.Color.x, v.Color.y, v.Color.z);
		OutputDebugStringA(buffer);
	}
	
	m_passConstants.World = XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(0.0f)));

	m_pConstantBuffer = std::make_unique<UploadBuffer<PassConstants>>(m_device.Get(), &m_passConstants, 256U);

}

void D3D12Engine::CompileShaders()
{

	HRESULT hr;

	hr = D3DCompileFromFile(L"Basic.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG, 0, vs_main.GetAddressOf(), errors.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		__debugbreak();
	}
	hr = D3DCompileFromFile(L"Basic.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG, 0, ps_main.GetAddressOf(), ps_main.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		__debugbreak();
	}

}

void D3D12Engine::CreateGraphicsPipeline()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ZeroMemory(&rootSignatureDesc, sizeof(CD3DX12_ROOT_SIGNATURE_DESC));

	CD3DX12_ROOT_PARAMETER rootParams[1];
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	// Empty for now, we don't have any SRVs or CBVs
	rootSignatureDesc.Init(_countof(rootParams), rootParams, 0, 0, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Describe the layout of our data
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2];
	ZeroMemory(&inputElementDesc, sizeof(D3D12_INPUT_ELEMENT_DESC));

	inputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0U, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12U, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), errors.GetAddressOf()));

	ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// Graphics Pipline
	D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPSODesc;
	ZeroMemory(&defaultPSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	defaultPSODesc.VS = { vs_main->GetBufferPointer(), vs_main->GetBufferSize() };
	defaultPSODesc.PS = { ps_main->GetBufferPointer(), ps_main->GetBufferSize() };
	defaultPSODesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
	defaultPSODesc.pRootSignature = m_rootSignature.Get();
	defaultPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	defaultPSODesc.NumRenderTargets = 1;
	defaultPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	defaultPSODesc.SampleMask = UINT_MAX;
	defaultPSODesc.SampleDesc.Count = 1;
	defaultPSODesc.SampleDesc.Quality = 0;
	defaultPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	defaultPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&defaultPSODesc, IID_PPV_ARGS(&m_defaultPipeline)));


	ZeroMemory(&m_rScissorsRect, sizeof(D3D12_RECT));
	m_rScissorsRect.top = 0; 
	m_rScissorsRect.left = 0;
	m_rScissorsRect.right = 1024;
	m_rScissorsRect.bottom = 768;

	ZeroMemory(&m_vViewPort, sizeof(D3D12_RECT));
	m_vViewPort.TopLeftX = 0;
	m_vViewPort.TopLeftY = 0;
	m_vViewPort.Width = 1024;
	m_vViewPort.Height = 768;
	m_vViewPort.MaxDepth = 1.0f;
}

void D3D12Engine::OnUpdate()
{
	// Nothing for now, eventually we will be controlling CBV Resources here.
	m_fAngle += 0.25f;

	m_passConstants.World = XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(m_fAngle)));
	//CopyMemory(m_pConstBufferData, &m_passConstants, sizeof(m_passConstants));
	m_pConstantBuffer->CopyData(&m_passConstants);
}

void D3D12Engine::OnRender()
{
	// Get the current back buffer index
	m_swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain3));
	m_iBackBufferIndex = m_swapChain3->GetCurrentBackBufferIndex();

	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	// Indicate that the back buffer will be used as a render target
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_rtvResources[m_iBackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE currentRTVHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	currentRTVHandle.Offset(m_iBackBufferIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	m_commandList->OMSetRenderTargets(1, &currentRTVHandle, 0, nullptr);

	float clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(currentRTVHandle, clear_color, 0, 0);


	m_commandList->SetPipelineState(m_defaultPipeline.Get());
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	m_commandList->RSSetScissorRects(1, &m_rScissorsRect);
	m_commandList->RSSetViewports(1, &m_vViewPort);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_commandList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer->GetAddress());

	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

	m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will be used to present
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_rtvResources[m_iBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());

	// Submit the command list to the GPU
	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// Present the Frame
	D3DApp::PresentFrame();

	// Sync the CPU/GPU to ensure that the GPU keeps up with the commands that was submitted by the CPU
	D3DApp::Sync();
}