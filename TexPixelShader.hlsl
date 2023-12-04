#include "BasicShaderHeader.hlsli"

float4 TexturePS(float4 pos : SV_POSITION, float3 color : COLOR, float2 uv : TEXCOORD) : SV_TARGET
{
    return float4(tex.Sample(smp, uv));
}