#pragma once
#include "Shape.h"
class Triangle : public Shape
{
public:
	Triangle(int x, int y, int length, ID3D12Device* device, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
};

