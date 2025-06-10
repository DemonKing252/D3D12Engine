#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include "UploadBuffer.h"
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

	UINT VertexCount;
	UINT IndexCount;
};

class GeometryGenerator
{
public:
	static GeometryData CreateTriangle(float width, float height);
	static GeometryData CreateQuad(float width, float height);
	static GeometryData CreateBox(float width, float height, float depth);
	static GeometryData CreatePyramid(float width, float height, float depth);
	
};

