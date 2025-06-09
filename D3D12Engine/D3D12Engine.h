#pragma once
#include "D3DApp.h"
#include "GeometryGenerator.h"
#include "SceneNode.h"
#include "d3dHelper.h"
#include <memory>
#include <unordered_map>
class Win32App;
class D3D12Engine : public D3DApp
{
private:
	UINT m_iBackBufferIndex = 0;

	ComPtr<ID3DBlob> errors;
	ComPtr<ID3DBlob> vs_main;
	ComPtr<ID3DBlob> ps_main;

	ComPtr<ID3DBlob> signature;

	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_defaultPipeline;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_meshGeometryMap;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materialMap;
	std::shared_ptr<UploadBuffer<PassConstants>> m_pConstantBuffer;
	PassConstants m_passConstants;

	SceneNode* m_pSceneHierarchy;
	
	ComPtr<ID3D12DescriptorHeap> m_pCBVDescriptorHeap;

	D3D12_VIEWPORT m_vViewPort;
	D3D12_RECT m_rScissorsRect;

	void* m_pConstBufferData;
	float m_fAngle = 0.0f;
	
	static D3D12Engine* s_pInstance;
public:
	std::shared_ptr<UploadBuffer<PassConstants>> GetConstantBuffer() const;
	static D3D12Engine* GetApp()
	{
		return s_pInstance;
	}
	static void Init(Win32App& app)
	{
		if (s_pInstance == nullptr)
			s_pInstance = new D3D12Engine(app);
	}

	D3D12Engine(Win32App& win32App);

	PassConstants& GetPassConstants();

	void CreateMaterials();
	void CreateGeometry();
	void CompileShaders();
	void CreateGraphicsPipeline();
	void BuildPassConstantResources();
	void CreateSceneGraph();

	void OnUpdate() override;
	void OnRender() override;

};

