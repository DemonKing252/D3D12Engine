#include "GeometryGenerator.h"

MeshGeometry GeometryGenerator::CreateTriangle()
{
    MeshGeometry triangleMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.0f, +0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
    };

    triangleMeshGeo.m_vVertices = verticies;
    return triangleMeshGeo;
}
