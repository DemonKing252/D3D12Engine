#include "D3D12Engine.h"
#include "Win32App.h"

D3D12Engine* D3D12Engine::s_pInstance = nullptr;

std::shared_ptr<UploadBuffer<PassConstants>> D3D12Engine::GetConstantBuffer() const
{
	return m_pConstantBuffer;
}
PassConstants& D3D12Engine::GetPassConstants()
{
	return m_passConstants;
}

D3D12Engine::D3D12Engine(Win32App& win32App) : D3DApp(win32App)
{
	this->D3DApp::InitializeD3D();
	this->D3DApp::BuildRenderTarget();
	this->Create3DCamera();
	this->CompileShaders();
	this->CreateGraphicsPipeline();
	this->CreateMaterials();
	this->CreateGeometry();
	this->CreateSceneGraph();
	this->BuildPassConstantResources();
}

void D3D12Engine::Create3DCamera()
{
	m_pCamera = std::make_unique<Camera>(90.0f, 0.01f, 1000.0f, 1024.0f / 768.0f);
	m_pCamera->SetLens(XMFLOAT3(0.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

}

void D3D12Engine::CreateMaterials()
{
	auto darkOliveGreen = std::make_unique<Material>();
	darkOliveGreen->Name = "oliveGreen";
	darkOliveGreen->DiffuseAlbedo = XMFLOAT4(0.6f, 1.0f, 0.2f, 1.000f);
	m_materialMap[darkOliveGreen->Name] = std::move(darkOliveGreen);

	auto cyan = std::make_unique<Material>();
	cyan->Name = "cyan";
	cyan->DiffuseAlbedo = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	m_materialMap[cyan->Name] = std::move(cyan);

	auto magenta = std::make_unique<Material>();
	magenta->Name = "magenta";
	magenta->DiffuseAlbedo = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	m_materialMap[magenta->Name] = std::move(magenta);
}

void D3D12Engine::CreateGeometry()
{
	// Triangle Mesh
	GeometryData triangleMeshData = GeometryGenerator::CreateTriangle(0.25f, 0.25f);
	
	auto triangleVertexBufferResource = new UploadBuffer<Vertex>(m_device.Get(), triangleMeshData.m_vVertices.data(), triangleMeshData.VertexSizeInBytes);	
	auto triangleIndexBufferResource = new UploadBuffer<UINT>(m_device.Get(), triangleMeshData.m_vIndicies.data(), triangleMeshData.IndexSizeInBytes);
	
	auto triangleMesh = std::make_unique<MeshGeometry>();
	triangleMesh->Name = "triangle";
	triangleMesh->VertexBufferGPU = d3dHelper::VertexBuffer<Vertex>(triangleVertexBufferResource, triangleMeshData.VertexSizeInBytes);
	triangleMesh->IndexBufferGPU = d3dHelper::IndexBuffer<UINT>(triangleIndexBufferResource, triangleMeshData.IndexSizeInBytes);
	triangleMesh->VertexCount = triangleMeshData.m_vVertices.size();
	triangleMesh->IndexCount = triangleMeshData.m_vIndicies.size();
	
	m_meshGeometryMap["triangle"] = std::move(triangleMesh);

	// Quad Mesh
	GeometryData quadMeshData = GeometryGenerator::CreateQuad(0.25f, 0.25f);
	
	auto quadVertexBufferResource = new UploadBuffer<Vertex>(m_device.Get(), quadMeshData.m_vVertices.data(), quadMeshData.VertexSizeInBytes);
	auto qudVertexBufferResource = new UploadBuffer<UINT>(m_device.Get(), quadMeshData.m_vIndicies.data(), quadMeshData.IndexSizeInBytes);
	
	auto quadMesh = std::make_unique<MeshGeometry>();
	quadMesh->Name = "quad";
	quadMesh->VertexBufferGPU = d3dHelper::VertexBuffer<Vertex>(quadVertexBufferResource, quadMeshData.VertexSizeInBytes);
	quadMesh->IndexBufferGPU = d3dHelper::IndexBuffer<UINT>(qudVertexBufferResource, quadMeshData.IndexSizeInBytes);
	quadMesh->VertexCount = quadMeshData.m_vVertices.size();
	quadMesh->IndexCount = quadMeshData.m_vIndicies.size();
	
	m_meshGeometryMap["quad"] = std::move(quadMesh);
	
}

void D3D12Engine::CreateSceneGraph()
{
	m_pSceneHierarchy = new SceneNode();
	
	auto* triangleA = new SceneNode(true, XMFLOAT3(-0.5f, 0.0f, 0.0f));
	triangleA->SetMeshGeometry(m_meshGeometryMap["triangle"].get());
	triangleA->SetMaterial(m_materialMap["cyan"].get());
	m_pSceneHierarchy->AddChild(triangleA);

	auto* triangleB = new SceneNode(true, XMFLOAT3(+0.5f, 0.0f, 0.0f));
	triangleB->SetMaterial(m_materialMap["oliveGreen"].get());
	triangleB->SetMeshGeometry(m_meshGeometryMap["quad"].get());

	auto* triangleC = new SceneNode(true, XMFLOAT3(+0.0f, -0.5f, 0.0f));
	triangleC->SetMaterial(m_materialMap["magenta"].get());
	triangleC->SetMeshGeometry(m_meshGeometryMap["triangle"].get());

	triangleB->AddChild(triangleC);
	
	m_pSceneHierarchy->AddChild(triangleB);
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

	m_pConstantBuffer = std::make_shared<UploadBuffer<PassConstants>>(m_device.Get(), &m_passConstants, SceneNode::Instances() * 256U);

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
		m_passConstants.Material.DiffuseAlbedo = XMFLOAT4(1.0f, 0.2f, 0.0f, 1.0f);

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
}

void D3D12Engine::OnUpdate()
{
	m_passConstants.ViewProj = m_pCamera->GetViewProj();
	m_pSceneHierarchy->Update(GetApp());
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

	m_pSceneHierarchy->Draw(m_commandList.Get());

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