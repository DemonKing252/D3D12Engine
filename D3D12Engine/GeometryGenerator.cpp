#include "GeometryGenerator.h"

GeometryData GeometryGenerator::CreateTriangle()
{
    GeometryData triangleMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.0f, +0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
    };

    std::vector<UINT> indicies = {
        0, 1, 2
    };

    triangleMeshGeo.m_vVertices = verticies;
    triangleMeshGeo.m_vIndicies = indicies;

    return triangleMeshGeo;
}
