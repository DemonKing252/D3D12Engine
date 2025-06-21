#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include "UploadBuffer.h"
#include "d3dHelper.h"
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
};

struct GeometryData
{
	std::vector<Vertex> m_vVertices;
	std::vector<UINT> m_vIndicies;
	UINT VertexSizeInBytes;
	UINT IndexSizeInBytes;
};

enum GeoAxis
{
	None,
	X,
	Y,
	Z
};

struct MeshGeometry
{
	std::string Name;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferGPU;
	D3D12_INDEX_BUFFER_VIEW IndexBufferGPU;

	std::unique_ptr<UploadBuffer<Vertex>> VertexBufferUpload;
	std::unique_ptr<UploadBuffer<UINT>> IndexBufferUpload;

	UINT VertexCount;
	UINT IndexCount;

	void Init(const char* name, ID3D12Device* device, GeometryData geoData) noexcept
	{
		this->VertexBufferUpload = std::make_unique<UploadBuffer<Vertex>>(device, geoData.m_vVertices.data(), geoData.VertexSizeInBytes, geoData.VertexSizeInBytes);
		this->IndexBufferUpload = std::make_unique<UploadBuffer<UINT>>(device, geoData.m_vIndicies.data(), geoData.IndexSizeInBytes, geoData.IndexSizeInBytes);
		this->Name = std::string(name);
		this->VertexBufferGPU = d3dHelper::VertexBuffer<Vertex>(this->VertexBufferUpload.get(), geoData.VertexSizeInBytes);
		this->IndexBufferGPU = d3dHelper::IndexBuffer<UINT>(this->IndexBufferUpload.get(), geoData.IndexSizeInBytes);
		this->VertexCount = geoData.m_vVertices.size();
		this->IndexCount = geoData.m_vIndicies.size();
	}
};

class GeometryGenerator
{
public:
	static GeometryData CreateTriangle(float width, float height);
	static GeometryData CreateQuad(float width, float height);
	static GeometryData CreateBox(float width, float height, float depth);
	static GeometryData CreatePyramid(float width, float height, float depth);
	static GeometryData CreateCylinder(int stackCount, int sliceCount, float topRadius, float bottomRadius, float height);
	static GeometryData CreateCurvyClinder(int stackCount, int sliceCount, float topRadius, float bottomRadius, float height);
	static GeometryData CreateTorus(int sliceCount, float radiusFromCenter, float ringRadius);
	static GeometryData CreateSphere(int sliceCount, int stackCount, float radius);
	
};

