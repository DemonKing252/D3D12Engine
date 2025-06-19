#include "GeometryGenerator.h"

GeometryData GeometryGenerator::CreateTriangle(float width, float height)
{
    GeometryData triangleMeshGeo;
    std::vector<Vertex> verticies =
    {
        { XMFLOAT3(-width, -height, 0.0f)},
        { XMFLOAT3(0.0f, +height, 0.0f)  },
        { XMFLOAT3(+width, -height, 0.0f)},
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
        { XMFLOAT3(+width, 0.0f, +depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(+width, 0.0f, -depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-width, 0.0f, -depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
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
        { XMFLOAT3(-width, +height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, +height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(+width, -height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(-width, -height, -depth), XMFLOAT3(1.0f, 1.0f, 0.0f) },

        { XMFLOAT3(-width, +height, +depth), XMFLOAT3(1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(+width, +height, +depth), XMFLOAT3(1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(+width, -height, +depth), XMFLOAT3(1.0f, 0.0f, 1.0f) },
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
        { XMFLOAT3(-width, -height, -depth)},
        { XMFLOAT3(-width, -height, +depth)},
        { XMFLOAT3(+width, -height, +depth)},
        { XMFLOAT3(+width, -height, -depth)},

        { XMFLOAT3(0.0f, height, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
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
/*
        winApp->m_fPhi -= (float)mouseDeltaX * 0.25f;
        winApp->m_fTheta -= (float)mouseDeltaY * 0.25f;

        winApp->m_fTheta = min(max(winApp->m_fTheta, -89.9f), 89.9f);

        XMFLOAT3 Eye;
        Eye.x = winApp->m_fRadius * cosf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));
        Eye.y = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fTheta));
        Eye.z = winApp->m_fRadius * sinf(XMConvertToRadians(winApp->m_fPhi)) * cosf(XMConvertToRadians(winApp->m_fTheta));
    */
GeometryData GeometryGenerator::CreateCylinder(int sliceCount, float topRadius, float bottomRadius, float height)
{
    GeometryData meshGeo;
    std::vector<Vertex> verticies;
    std::vector<UINT> indicies;    

    // Just for fun. We don't have texturing in yet so its easier to see the primitive distribution this way:
    XMFLOAT3 Colors[] = {
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 1.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 1.0f),
    };

    float angleRadians = 0.0f;
    float offset = XMConvertToRadians(360.0f / (float)sliceCount);

    for (UINT i = 0; i < sliceCount; i++)
    {

        XMFLOAT3 bottomLeft;
        bottomLeft.x = bottomRadius * cosf(angleRadians);
        bottomLeft.y = -height * 0.5f;
        bottomLeft.z = bottomRadius * sinf(angleRadians);

        XMFLOAT3 topLeft;
        topLeft.x = topRadius * cosf(angleRadians);
        topLeft.y = +height * 0.5f;
        topLeft.z = topRadius * sinf(angleRadians);

        XMFLOAT3 topRight;
        topRight.x = topRadius * cosf(angleRadians + offset);
        topRight.y = +height * 0.5f;
        topRight.z = topRadius * sinf(angleRadians + offset);

        XMFLOAT3 bottomRight;
        bottomRight.x = bottomRadius * cosf(angleRadians + offset);
        bottomRight.y = -height * 0.5f;
        bottomRight.z = bottomRadius * sinf(angleRadians + offset);
        
    
        verticies.push_back({XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[i % 3]});
        verticies.push_back({XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[i % 3] });
        verticies.push_back({XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[i % 3] });
        verticies.push_back({XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[i % 3] });

        angleRadians += offset;
    }
    // Top
    for (UINT i = 0; i < sliceCount; i++)
    {

        XMFLOAT3 topLeft;
        topLeft.x = topRadius * cosf(angleRadians);
        topLeft.y = +height * 0.5f;
        topLeft.z = topRadius * sinf(angleRadians);

        XMFLOAT3 topRight;
        topRight.x = topRadius * cosf(angleRadians + offset);
        topRight.y = +height * 0.5f;
        topRight.z = topRadius * sinf(angleRadians + offset);

        XMFLOAT3 topCenter;
        topCenter.x = 0.0f;
        topCenter.y = height * 0.5f;
        topCenter.z = 0.0f;

        verticies.push_back({ XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topCenter.x, topCenter.y, topCenter.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[i % 3] });

        angleRadians += offset;
    }

    // Bottom
    for (UINT i = 0; i < sliceCount; i++)
    {
        XMFLOAT3 bottomLeft;
        bottomLeft.x = bottomRadius * cosf(angleRadians);
        bottomLeft.y = -height * 0.5f;
        bottomLeft.z = bottomRadius * sinf(angleRadians);

        XMFLOAT3 bottomRight;
        bottomRight.x = bottomRadius * cosf(angleRadians + offset);
        bottomRight.y = -height * 0.5f;
        bottomRight.z = bottomRadius * sinf(angleRadians + offset);

        XMFLOAT3 topCenter;
        topCenter.x = 0.0f;
        topCenter.y = -height * 0.5f;
        topCenter.z = 0.0f;

        verticies.push_back({ XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topCenter.x, topCenter.y, topCenter.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[i % 3] });

        angleRadians += offset;
    }

    unsigned int index = 0;
    for (UINT i = 0; i < sliceCount; i++)
    {
        indicies.push_back(index);
        indicies.push_back(index+1);
        indicies.push_back(index+2);
        indicies.push_back(index+2);
        indicies.push_back(index+3);
        indicies.push_back(index);

        index += 4;
    }
    for (UINT i = 0; i < sliceCount; i++)
    {
        indicies.push_back(index);
        indicies.push_back(index + 1);
        indicies.push_back(index + 2);
        
        index += 3;
    }
    for (UINT i = 0; i < sliceCount; i++)
    {
        indicies.push_back(index);
        indicies.push_back(index + 1);
        indicies.push_back(index + 2);

        index += 3;
    }

    meshGeo.m_vVertices = verticies;
    meshGeo.m_vIndicies = indicies;
    meshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    meshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return meshGeo;
}