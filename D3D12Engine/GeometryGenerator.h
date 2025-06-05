#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace std;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

struct MeshGeometry
{
	std::vector<Vertex> m_vVertices;
};

class GeometryGenerator
{
public:
	static MeshGeometry CreateTriangle();

};

