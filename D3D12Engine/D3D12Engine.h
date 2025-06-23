#pragma once
#include "D3DApp.h"
#include "GeometryGenerator.h"
#include "SceneNode.h"
#include "d3dHelper.h"
#include <memory>
#include <unordered_map>
#include "Camera.h"
#include "DDSTextureLoader.h"
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

	ComPtr<ID3D12PipelineState> m_pOpaquePSO;
	ComPtr<ID3D12PipelineState> m_pWireFramePSO;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_meshGeometryMap;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materialMap;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureMap;	
	std::shared_ptr<UploadBuffer<ObjectPassConstants>> m_pObjectConstants;
	std::shared_ptr<UploadBuffer<FramePassConstants>> m_pFrameConstants;

	ObjectPassConstants m_objPassConstants;
	FramePassConstants m_frameConstants;

	SceneNode* m_pSceneHierarchy;
	

	D3D12_VIEWPORT m_vViewPort;
	D3D12_RECT m_rScissorsRect;

	void* m_pConstBufferData;
	float m_fAngle = 0.0f;
	
	static D3D12Engine* s_pInstance;
public:
	ComPtr<ID3D12DescriptorHeap> m_pCBVSRVDescriptorHeap;
	std::unique_ptr<Camera> m_pCamera;
	std::shared_ptr<UploadBuffer<ObjectPassConstants>> GetConstantBuffer() const;
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

	ObjectPassConstants& GetPassConstants();
	ComPtr<ID3D12DescriptorHeap> GetCBVSRVDescriptorHeap();

	void Create3DCamera();
	void CreateMaterials();
	void CreateTextures();
	void CreateDescriptorHeaps();
	void LoadTextures();
	void CreateGeometry();
	void CompileShaders();
	void CreateGraphicsPipeline();
	void BuildPassConstantResources();
	void CreateSceneGraph();

	void OnUpdate() override;
	void OnRender() override;

};

