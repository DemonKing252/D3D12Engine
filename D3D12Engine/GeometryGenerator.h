#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include "UploadHeap.h"
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

struct GeometryData
{
	std::vector<Vertex> m_vVertices;
	std::vector<UINT> m_vIndicies;
};

struct MeshGeometry
{
	std::string Name;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferGPU;
	D3D12_INDEX_BUFFER_VIEW IndexBufferGPU;

	UINT VertexCount;
	UINT IndexCount;
};

class GeometryGenerator
{
public:
	static GeometryData CreateTriangle();

};

