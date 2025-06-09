#include "SceneNode.h"
#include "D3D12Engine.h"
std::uint64_t SceneNode::s_iNumberOfInstances = 0;

SceneNode::SceneNode(bool isRenderable, XMFLOAT3 Position, XMFLOAT3 Scale, XMFLOAT3 Rotation)
	: Position(Position),
	  Scale(Scale),
	  Rotation(Rotation),
	  m_bIsRenderable(isRenderable),
      m_pMaterial(nullptr)
{
	if (m_bIsRenderable)
		SceneNode::s_iNumberOfInstances++;
}

SceneNode::SceneNode()
	: Position(XMFLOAT3(0.f, 0.f, 0.f)),
	  Scale(XMFLOAT3(0.f, 0.f, 0.f)),
	  Rotation(XMFLOAT3(0.f, 0.f, 0.f)),
	  m_bIsRenderable(false),
	  m_pMaterial(nullptr)
{
	if (m_bIsRenderable)
		SceneNode::s_iNumberOfInstances++;
}

SceneNode::~SceneNode()
{
}

std::uint64_t SceneNode::RenderableNodeIndex() const
{
	return m_iRenderableNodeIndex;
}

std::vector<SceneNode*> SceneNode::GetChildren()
{
	return m_children;
}

std::uint64_t SceneNode::Instances()
{
	return s_iNumberOfInstances;
}

bool SceneNode::IsRenderable() const
{
	return m_bIsRenderable;
}

void SceneNode::SetMaterial(Material* mat)
{
	this->m_pMaterial = mat;
}

void SceneNode::SetMeshGeometry(MeshGeometry* mesh)
{
	this->m_pMeshGeo = mesh;
}

void SceneNode::SetParent(SceneNode* parent)
{
	this->m_pParentNode = parent;
}

void SceneNode::SetRenderableNodeIndex(const std::uint64_t index)
{
	this->m_iRenderableNodeIndex = index;
}

void SceneNode::SetGraphicsHandleToConstantBuffer(const CD3DX12_GPU_DESCRIPTOR_HANDLE handle)
{
	this->GPUDescriptorHandle = handle;
}

void SceneNode::AddChild(SceneNode* rhs)
{
	rhs->SetParent(this);
	rhs->Position = XMFLOAT3(this->Position.x + rhs->Position.x, this->Position.y + rhs->Position.y, this->Position.z + rhs->Position.z);
	m_children.push_back(rhs);
}

void SceneNode::Update(D3D12Engine* d3dApp)
{
	if (m_pMaterial != nullptr)
		d3dApp->GetPassConstants().Material.DiffuseAlbedo = m_pMaterial->DiffuseAlbedo;
	
	d3dApp->GetPassConstants().World = XMMatrixTranspose(XMMatrixTranslation(Position.x, Position.y, Position.z));
	d3dApp->GetConstantBuffer()->CopyData(m_iRenderableNodeIndex, &d3dApp->GetPassConstants());

	for (auto& c : m_children)
		c->Update(d3dApp);
}

void SceneNode::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	if (m_bIsRenderable)
	{	
		pCmdList->IASetVertexBuffers(0, 1, &m_pMeshGeo->VertexBufferGPU);
		pCmdList->IASetIndexBuffer(&m_pMeshGeo->IndexBufferGPU);
		pCmdList->SetGraphicsRootDescriptorTable(0, GPUDescriptorHandle);		
		pCmdList->DrawIndexedInstanced(m_pMeshGeo->IndexCount, 1, 0, 0, 0);
	}
		
	for (auto& c : m_children)
		c->Draw(pCmdList);
}
