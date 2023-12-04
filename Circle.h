#pragma once
#include "Shape.h"
class Circle : public Shape
{
public:
	Circle(int x, int y, int r, ID3D12Device* device, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
};

