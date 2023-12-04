#include "BasicShaderHeader.hlsli"

float4 BasicPS(float4 pos : SV_POSITION, float3 color: COLOR, float2 uv : TEXCOORD) : SV_TARGET
{
	return float4(color.rgb, 1.0f);
}