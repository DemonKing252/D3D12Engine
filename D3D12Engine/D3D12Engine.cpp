#include "D3D12Engine.h"
#include "Win32App.h"

D3D12Engine::D3D12Engine(Win32App& win32App) : D3DApp(win32App)
{
	this->D3DApp::InitializeD3D();
	this->D3DApp::BuildRenderTarget();
	this->CompileShaders();
	this->CreateGraphicsPipeline();
	this->CreateSceneGraph();
	this->BuildPassConstantResources();
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
	

	// Create the scene graph first then create the CBV based on the number of descriptors we need

	// Liam's Notes:
	// There's 3 different ways we could go about this:	 
	/*
		Option A (Current one) - Create 2 resources with the correct alignment requirements that the GPU requires, create a Descriptor heap of size 2 and offset to that resource
		Option B - Create 1 resource of double the size of a SINGLE constant buffer (still following alignment requirements), and store each CB using an offset instead of 2 different resource, and continue using a descriptor heap of size 2
		Option C - Use 2 parallel resources (without a constant buffer view descriptor) and set the accordingly using cmdList. Note that this requires you to modify the root signature parameters as well
		Option D - (not efficent) - Use root constants which doesn't require a descriptor heap. However you are also resonsible for tracking each root location in the cbuffer. This becomes very messy with a bigger project.
	*/

	

}

void D3D12Engine::CreateSceneGraph()
{
	m_pSceneHierarchy = new SceneNode();
	
	auto* triangleA = new SceneNode(true, XMFLOAT3(-0.5f, 0.0f, 0.0f));
	m_pSceneHierarchy->AddChild(triangleA);

	auto* triangleB = new SceneNode(true, XMFLOAT3(+0.5f, 0.0f, 0.0f));
	auto* triangleC = new SceneNode(true, XMFLOAT3(+0.0f, -0.25f, 0.0f));

	triangleB->AddChild(triangleC);
	
	m_pSceneHierarchy->AddChild(triangleB);
	
	char buffer[200];
	sprintf_s(buffer, "Num Instances: %i\n", SceneNode::Instances());

	OutputDebugStringA(buffer);
	for (auto c : m_pSceneHierarchy->GetChildren())
	{
		char buffer[220];
		sprintf_s(buffer, "[Child index: %i], [x: %f], [y: %f]\n", c->RenderableNodeIndex(), c->Position.x, c->Position.y);

		OutputDebugStringA(buffer);

		for (auto c_of_c : c->GetChildren())
		{

			sprintf_s(buffer, "[Child index: %i], [x: %f], [y: %f]\n", c_of_c->RenderableNodeIndex(), c_of_c->Position.x, c_of_c->Position.y);

			OutputDebugStringA(buffer);
		}
	}

	//OutputDebugStringA(buffer);
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

	CD3DX12_DESCRIPTOR_RANGE cbvRange;
	cbvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);// last arg is base register --> cbuffer : register(b0)

	CD3DX12_ROOT_PARAMETER rootParams[1];
	rootParams[0].InitAsDescriptorTable(1, &cbvRange, D3D12_SHADER_VISIBILITY_ALL);
	//rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

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

void D3D12Engine::BuildPassConstantResources()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDescHeap;
	ZeroMemory(&cbvDescHeap, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	cbvDescHeap.NumDescriptors = SceneNode::Instances();
	cbvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	
	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvDescHeap, IID_PPV_ARGS(m_pCBVDescriptorHeap.GetAddressOf())));


	m_pConstantBuffer = std::make_unique<UploadBuffer<PassConstants>>(m_device.Get(), &m_passConstants, SceneNode::Instances() * 256U);

	//m_passConstants.World = XMMatrixTranspose(XMMatrixTranslation(-0.5f, 0.0f, 0.0f));
	//m_pConstantBuffer->CopyData(0, &m_passConstants);
	//
	//m_passConstants.World = XMMatrixTranspose(XMMatrixTranslation(+0.5f, 0.0f, 0.0f));
	//m_pConstantBuffer->CopyData(1, &m_passConstants);

	const UINT cbvDescriptorHandleIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT drawableIndex = 0;

	auto allocate_resource_memory = [this, &drawableIndex, &cbvDescriptorHandleIncrementSize] (SceneNode* c)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		ZeroMemory(&cbvDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));

		cbvDesc.BufferLocation = m_pConstantBuffer->GetAddress(drawableIndex);
		cbvDesc.SizeInBytes = 256U;

		CD3DX12_CPU_DESCRIPTOR_HANDLE sceneNodeCPUHandle(m_pCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		sceneNodeCPUHandle.Offset(drawableIndex, cbvDescriptorHandleIncrementSize);

		CD3DX12_GPU_DESCRIPTOR_HANDLE sceneNodeGPUHandle(m_pCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		sceneNodeGPUHandle.Offset(drawableIndex, cbvDescriptorHandleIncrementSize);

		m_device->CreateConstantBufferView(&cbvDesc, sceneNodeCPUHandle);

		m_passConstants.World = XMMatrixTranspose(
			XMMatrixTranslation(
				c->Position.x,
				c->Position.y,
				c->Position.z
			));

		m_pConstantBuffer->CopyData(
			drawableIndex,
			&m_passConstants
		);
		c->SetRenderableNodeIndex(drawableIndex);

		c->SetGraphicsHandleToConstantBuffer(sceneNodeGPUHandle);
		++drawableIndex;
	};

	
	for (auto& node : m_pSceneHierarchy->GetChildren())
	{
		if (!node->IsRenderable())
			continue;

		allocate_resource_memory(node);

		for (auto node_of_node : node->GetChildren())
		{
			allocate_resource_memory(node_of_node);
		}
	}

	for (auto c : m_pSceneHierarchy->GetChildren())
	{
		char buffer[220];
		sprintf_s(buffer, "[Child index: %i], [x: %f], [y: %f]\n", c->RenderableNodeIndex(), c->Position.x, c->Position.y);

		OutputDebugStringA(buffer);
	}

	int x = 1;

	//CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_pCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//
	//cbvDesc.BufferLocation = m_pConstantBuffer->GetAddress(1);
	//m_device->CreateConstantBufferView(&cbvDesc, cbvHandle);
}

void D3D12Engine::OnUpdate()
{
}

void D3D12Engine::OnRender()
{
	// get the current back buffer index
	m_swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain3));
	m_iBackBufferIndex = m_swapChain3->GetCurrentBackBufferIndex();

	ThrowIfFailed( m_commandAllocator->Reset() );
	ThrowIfFailed( m_commandList->Reset(m_commandAllocator.Get(), nullptr) );

	// indicate that the back buffer will be used as a render target
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

	m_commandList->SetDescriptorHeaps(1, m_pCBVDescriptorHeap.GetAddressOf());
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

	m_pSceneHierarchy->Draw(m_commandList.Get());

	// next descriptor
	//cbvGPUHandle.Offset(1, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//
	//m_commandList->SetGraphicsRootDescriptorTable(0, cbvGPUHandle);
	//
	//// draw call #2
	//m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will be used to present
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_rtvResources[m_iBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed( m_commandList->Close() );

	// submit the command list to the GPU
	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// present the Frame
	D3DApp::PresentFrame();

	// sync the CPU/GPU to ensure that the GPU keeps up with the commands that was submitted by the CPU
	D3DApp::Sync();
}