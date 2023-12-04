#include "BasicShaderHeader.hlsli"

Output BasicVS(float4 pos : POSITION, float3 color: COLOR, float2 uv: TEXCOORD)
{
    Output output;
    matrix worldView = mul(view, world);
    output.pos = mul(worldView, pos);
    output.color = color;
    output.uv = uv;
	return output;
}