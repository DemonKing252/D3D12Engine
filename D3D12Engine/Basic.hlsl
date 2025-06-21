struct VSVertexIn
{
    float3 pos : POSITION;
    float3 col : COLOR;
};
struct PSVertexIn
{    
    float4 pos : SV_Position;
    float3 col : COLOR;
};
struct VertexOut
{
    float4 pos : SV_Position;
    float3 col : COLOR;
};

struct Material
{
    float4 DiffuseAlbedo;
};

cbuffer ObjectPassConstants : register(b0)  // Per Object
{
    float4x4 gWorld;
    Material gMaterial;
}
cbuffer FramePassConstants : register(b1) // Per Frame
{
    float4x4 gViewProj;
}

VertexOut VSMain(VSVertexIn vIn)
{
    // Just pass the position and color to the Pixel Shader, we aren't doing any transformations yet
    
    float4 ModelP = mul(float4(vIn.pos, 1.0f), gWorld);
    float4 WorldP = mul(ModelP, gViewProj);
    
    VertexOut vOut;    
    
    vOut.pos = WorldP;
    vOut.col = vIn.col;
    
    return vOut;
}

float4 PSMain(PSVertexIn vIn) : SV_Target
{
    // Return vertex color * Material Diffuse
    return float4(gMaterial.DiffuseAlbedo.xyz * vIn.col, 1.0f);
}