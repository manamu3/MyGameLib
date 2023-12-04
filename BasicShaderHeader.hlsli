struct Output
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer cbuff0 : register(b0)
{
    matrix view;
}

cbuffer cbuff1 : register(b1)
{
    matrix world;
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);