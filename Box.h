#pragma once
#include "Shape.h"

class Box : public Shape
{
public:
	Box(int sx, int sy, int ex, int ey, ID3D12Device* device, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
};

