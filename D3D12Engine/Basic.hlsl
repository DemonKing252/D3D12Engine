struct VSVertexIn
{
    float2 pos : POSITION;
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

cbuffer PassConstants : register(b0)
{
    float4x4 gWorld;
}

VertexOut VSMain(VSVertexIn vIn)
{
    // Just pass the position and color to the Pixel Shader, we aren't doing any transformations yet
    
    float4 WorldP = mul(float4(vIn.pos, 0.0f, 1.0f), gWorld);
    
    VertexOut vOut;    
    
    vOut.pos = WorldP;
    vOut.col = vIn.col;
    
    return vOut;
}

float4 PSMain(PSVertexIn vIn) : SV_Target
{
    // Return the color that was recieved
    return float4(vIn.col, 1.0f);
}