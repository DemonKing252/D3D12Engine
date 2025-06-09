#include "GeometryGenerator.h"

GeometryData GeometryGenerator::CreateTriangle(float width, float height)
{
    GeometryData triangleMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, -height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.0f, +height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+width, -height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
    };

    std::vector<UINT> indicies = {
        0, 1, 2
    };

    triangleMeshGeo.m_vVertices = verticies;
    triangleMeshGeo.m_vIndicies = indicies;
    triangleMeshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    triangleMeshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return triangleMeshGeo;
}

GeometryData GeometryGenerator::CreateQuad(float width, float height)
{
    GeometryData quadMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, +height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+width, +height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+width, -height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-width, -height, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
    };

    std::vector<UINT> indicies = {
        0, 1, 2,
        2, 3, 0
    };

    quadMeshGeo.m_vVertices = verticies;
    quadMeshGeo.m_vIndicies = indicies;
    quadMeshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    quadMeshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return quadMeshGeo;
}
