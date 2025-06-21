#pragma once
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>
#include "d3dx12.h"
#include "GeometryGenerator.h"
#include "d3dHelper.h"
using namespace std;
using namespace DirectX;

class D3D12Engine;
class SceneNode
{
private:
	bool m_bIsRenderable;
	static std::uint64_t s_iNumberOfInstances;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle;
	std::uint64_t m_iRenderableNodeIndex = 0;
	std::vector<SceneNode*> m_children;
	SceneNode* m_pParentNode;
	MeshGeometry* m_pMeshGeo;
	Material* m_pMaterial;
public:
	SceneNode(bool isRenderable, XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3 Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f));
	SceneNode();
	~SceneNode();

	// These are public for now, eventually we will make them private, and allow the developer to freely modify them, whenever something is modifed, the resource data is reuploaded
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;

	std::uint64_t RenderableNodeIndex() const;
	std::vector<SceneNode*> GetChildren();
	static std::uint64_t Instances();
	bool IsRenderable() const;

	void SetMaterial(Material* mat);
	void SetMeshGeometry(MeshGeometry* mesh);
	void SetParent(SceneNode* parent);
	void SetRenderableNodeIndex(const std::uint64_t index);
	void SetGraphicsHandleToConstantBuffer(const CD3DX12_GPU_DESCRIPTOR_HANDLE handle);
	void AddChild(SceneNode* rh);

	void Update(D3D12Engine* d3dApp);
	void Draw(ID3D12GraphicsCommandList* pCmdList);
};

