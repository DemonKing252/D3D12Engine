struct VSVertexIn
{
    float3 pos : POSITION;
    float3 col : COLOR;
    float2 uv : UVCOORD;
};
struct PSVertexIn
{    
    float4 pos : SV_Position;
    float3 col : COLOR;
    float2 uv : UVCOORD;
};
struct VertexOut
{
    float4 pos : SV_Position;
    float3 col : COLOR;
    float2 uv : UVCOORD;
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

SamplerState sample : register(s0);
Texture2D tex : register(t0);

VertexOut VSMain(VSVertexIn vIn)
{
    // Just pass the position and color to the Pixel Shader, we aren't doing any transformations yet
    
    float4 ModelP = mul(float4(vIn.pos, 1.0f), gWorld);
    float4 WorldP = mul(ModelP, gViewProj);
    
    VertexOut vOut;    
    
    vOut.pos = WorldP;
    vOut.col = vIn.col;
    vOut.uv = vIn.uv;
    
    return vOut;
}

float4 PSMain(PSVertexIn vIn) : SV_Target
{
    float4 texUV = tex.Sample(sample, vIn.uv);
    
    float3 vertex_color = gMaterial.DiffuseAlbedo.xyz * vIn.col;
    
    // Return vertex color * Material Diffuse
    return float4(vertex_color, 1.0f);
}