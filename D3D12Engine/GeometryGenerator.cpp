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

GeometryData GeometryGenerator::CreateQuad(float width, float depth)
{
    GeometryData quadMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, 0.0f, +depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(+width, 0.0f, +depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, 0.0f, -depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-width, 0.0f, -depth), XMFLOAT3(1.0f, 0.0f, 1.0f) },
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

GeometryData GeometryGenerator::CreateBox(float width, float height, float depth)
{
    GeometryData meshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, +height, -depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(+width, +height, -depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, -height, -depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-width, -height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },

        { XMFLOAT3(-width, +height, +depth), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(+width, +height, +depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, -height, +depth), XMFLOAT3(0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-width, -height, +depth), XMFLOAT3(1.0f, 0.0f, 1.0f) },
    };

    std::vector<UINT> indicies = {
        // front face
        0, 1, 2,
        2, 3, 0,

        // top face
        0, 4, 5,
        5, 1, 0,

        // bottom face
        3, 2, 6,
        6, 7, 3,

        // left face
        7, 4, 0,
        0, 3, 7,

        // right face
        1, 5, 6,
        6, 2, 1,

        // back face
        5, 4, 7,
        7, 6, 5
    };

    meshGeo.m_vVertices = verticies;
    meshGeo.m_vIndicies = indicies;
    meshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    meshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return meshGeo;
}

GeometryData GeometryGenerator::CreatePyramid(float width, float height, float depth)
{
    GeometryData meshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, -height, -depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-width, -height, +depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, -height, +depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(+width, -height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },

        { XMFLOAT3(0.0f, height, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
    };

    std::vector<UINT> indicies = {
        // front face
        0, 4, 3,
        // left face
        1, 4, 0,
        // right face
        3, 4, 2,
        // back face
        2, 4, 1,
        
        // base
        0, 3, 2,
        2, 1, 0
    };

    meshGeo.m_vVertices = verticies;
    meshGeo.m_vIndicies = indicies;
    meshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    meshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return meshGeo;
}
