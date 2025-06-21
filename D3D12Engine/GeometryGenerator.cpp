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
GeometryData GeometryGenerator::CreateCylinder(int stackCount, int sliceCount, float topRadius, float bottomRadius, float height)
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
    float offsetX = XMConvertToRadians(360.0f / static_cast<float>(sliceCount));
    float offsetY = height / static_cast<float>(stackCount);
    float heightY = -height * 0.5f;

    for (UINT i = 0; i < stackCount; i++)
    {
        for (UINT j = 0; j < sliceCount; j++)
        {
            XMFLOAT3 bottomLeft;
            bottomLeft.x = bottomRadius * cosf(angleRadians);
            bottomLeft.y = heightY;
            bottomLeft.z = bottomRadius * sinf(angleRadians);

            XMFLOAT3 topLeft;
            topLeft.x = topRadius * cosf(angleRadians);
            topLeft.y = heightY+offsetY;
            topLeft.z = topRadius * sinf(angleRadians);

            XMFLOAT3 topRight;
            topRight.x = topRadius * cosf(angleRadians + offsetX);
            topRight.y = heightY + offsetY;
            topRight.z = topRadius * sinf(angleRadians + offsetX);

            XMFLOAT3 bottomRight;
            bottomRight.x = bottomRadius * cosf(angleRadians + offsetX);
            bottomRight.y = heightY;
            bottomRight.z = bottomRadius * sinf(angleRadians + offsetX);
            
            int colorIndex = (i + j) % 3;
            verticies.push_back({ XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[colorIndex] });

            angleRadians += offsetX;
        }
        heightY += offsetY;
    }
    // Top
    for (UINT i = 0; i < sliceCount; i++)
    {

        XMFLOAT3 topLeft;
        topLeft.x = topRadius * cosf(angleRadians);
        topLeft.y = +height * 0.5f;
        topLeft.z = topRadius * sinf(angleRadians);

        XMFLOAT3 topRight;
        topRight.x = topRadius * cosf(angleRadians + offsetX);
        topRight.y = +height * 0.5f;
        topRight.z = topRadius * sinf(angleRadians + offsetX);

        XMFLOAT3 topCenter;
        topCenter.x = 0.0f;
        topCenter.y = height * 0.5f;
        topCenter.z = 0.0f;

        verticies.push_back({ XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topCenter.x, topCenter.y, topCenter.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[i % 3] });

        angleRadians += offsetX;
    }

    // Bottom
    for (UINT i = 0; i < sliceCount; i++)
    {
        XMFLOAT3 bottomLeft;
        bottomLeft.x = bottomRadius * cosf(angleRadians);
        bottomLeft.y = -height * 0.5f;
        bottomLeft.z = bottomRadius * sinf(angleRadians);

        XMFLOAT3 bottomRight;
        bottomRight.x = bottomRadius * cosf(angleRadians + offsetX);
        bottomRight.y = -height * 0.5f;
        bottomRight.z = bottomRadius * sinf(angleRadians + offsetX);

        XMFLOAT3 topCenter;
        topCenter.x = 0.0f;
        topCenter.y = -height * 0.5f;
        topCenter.z = 0.0f;

        verticies.push_back({ XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(topCenter.x, topCenter.y, topCenter.z), Colors[i % 3] });
        verticies.push_back({ XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[i % 3] });

        angleRadians += offsetX;
    }

    UINT index = 0;
    for (UINT i = 0; i < sliceCount*stackCount; i++)
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

GeometryData GeometryGenerator::CreateTorus(int sliceCount, float radiusFromCenter, float ringRadius)
{
    
    GeometryData meshGeo;
    std::vector<Vertex> verticies;
    std::vector<UINT> indicies;
    
    XMFLOAT3 Colors[] = {
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 1.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 1.0f),
        XMFLOAT3(1.0f, 0.0f, 1.0f),
        XMFLOAT3(1.0f, 1.0f, 0.0f),
        XMFLOAT3(1.0f, 0.0f, 1.0f),
    };
    
    /*
    theta = Angle from Center
    phi = Angle from Center of Current Ring
    */
    
    float R = radiusFromCenter;
    float r = ringRadius;
    float theta = 0.0f;
    float phi = -90.0f;
    float offset = XMConvertToRadians(360.0f / (float)sliceCount);

    for (UINT i = 0; i < sliceCount; i++)
    {

        for (UINT j = 0; j < sliceCount; j++)
        {
            XMFLOAT3 bottomLeft;
            bottomLeft.x = (R + r * cosf(phi)) * cosf(theta);
            bottomLeft.z = (R + r * cosf(phi)) * sinf(theta);
            bottomLeft.y = r * sinf(phi);

            XMFLOAT3 topLeft;
            topLeft.x = (R + r * cosf(phi)) * cosf(theta+offset);
            topLeft.z = (R + r * cosf(phi)) * sinf(theta+offset);
            topLeft.y = r * sinf(phi);

            XMFLOAT3 topRight;
            topRight.x = (R + r * cosf(phi+offset)) * cosf(theta + offset);
            topRight.z = (R + r * cosf(phi+offset)) * sinf(theta + offset);
            topRight.y = r * sinf(phi + offset);

            XMFLOAT3 bottomRight;
            bottomRight.x = (R + r * cosf(phi+offset)) * cosf(theta);
            bottomRight.z = (R + r * cosf(phi+offset)) * sinf(theta);
            bottomRight.y = r * sinf(phi+offset);
            
            int colorIndex = j % 6;

            verticies.push_back({ XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[colorIndex] });
            phi += offset;

        }
        theta += offset;
    }
    
    UINT index = 0;
    for (UINT i = 0; i < sliceCount * sliceCount; i++)
    {
        indicies.push_back(index);
        indicies.push_back(index + 3);
        indicies.push_back(index + 2);
        indicies.push_back(index + 2);
        indicies.push_back(index + 1);
        indicies.push_back(index);

        index += 4;
    }
    
    meshGeo.m_vVertices = verticies;
    meshGeo.m_vIndicies = indicies;
    meshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    meshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);
    
    return meshGeo;
}

GeometryData GeometryGenerator::CreateSphere(int sliceCount, int stackCount, float radius)
{
    GeometryData meshGeo;
    std::vector<Vertex> verticies;
    std::vector<UINT> indicies;

    XMFLOAT3 Colors[] = {
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 1.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 1.0f),
    };

    /*
    theta = Y
    phi = X
    */

    float theta = 0.0f;
    float phi = -90.0f;
    float offset = 360.0f / (float)sliceCount;
    for (UINT i = 0; i < sliceCount; i++)
    {
        for (UINT j = 0; j < stackCount; j++)
        {
            XMFLOAT3 bottomLeft;
            bottomLeft.x = radius * cosf(XMConvertToRadians(phi)) * cosf(XMConvertToRadians(theta));
            bottomLeft.y = radius * sinf(XMConvertToRadians(theta));
            bottomLeft.z = radius * sinf(XMConvertToRadians(phi)) * cosf(XMConvertToRadians(theta));

            XMFLOAT3 topLeft;
            topLeft.x = radius * cosf(XMConvertToRadians(phi)) * cosf(XMConvertToRadians(theta + offset));
            topLeft.y = radius * sinf(XMConvertToRadians(theta + offset));
            topLeft.z = radius * sinf(XMConvertToRadians(phi)) * cosf(XMConvertToRadians(theta + offset));

            XMFLOAT3 topRight;
            topRight.x = radius * cosf(XMConvertToRadians(phi + offset)) * cosf(XMConvertToRadians(theta + offset));
            topRight.y = radius * sinf(XMConvertToRadians(theta + offset));
            topRight.z = radius * sinf(XMConvertToRadians(phi + offset)) * cosf(XMConvertToRadians(theta + offset));

            XMFLOAT3 bottomRight;
            bottomRight.x = radius * cosf(XMConvertToRadians(phi+offset)) * cosf(XMConvertToRadians(theta));
            bottomRight.y = radius * sinf(XMConvertToRadians(theta));
            bottomRight.z = radius * sinf(XMConvertToRadians(phi+offset)) * cosf(XMConvertToRadians(theta));

            int colorIndex = (i + j) % 3;
            verticies.push_back({ XMFLOAT3(bottomLeft.x, bottomLeft.y, bottomLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topLeft.x, topLeft.y, topLeft.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(topRight.x, topRight.y, topRight.z), Colors[colorIndex] });
            verticies.push_back({ XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z), Colors[colorIndex] });
            phi += offset;
        }      
        theta += offset;
    }

    UINT index = 0;
    for (UINT i = 0; i < sliceCount * stackCount; i++)
    {
        indicies.push_back(index);
        indicies.push_back(index + 1);
        indicies.push_back(index + 2);
        indicies.push_back(index + 2);
        indicies.push_back(index + 3);
        indicies.push_back(index);

        index += 4;
    }

    meshGeo.m_vVertices = verticies;
    meshGeo.m_vIndicies = indicies;
    meshGeo.VertexSizeInBytes = verticies.size() * sizeof(Vertex);
    meshGeo.IndexSizeInBytes = indicies.size() * sizeof(UINT);

    return meshGeo;
}
