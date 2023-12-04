#include "Triangle.h"
#include <cmath>

Triangle::Triangle(int x, int y, int length, ID3D12Device* device, DirectX::XMFLOAT3 color) {
	float height = std::sqrtf(3) / 2.0f * (float)length;

	std::vector<VertexData> vertices(3);
	vertices[0].position = { 0, -height / 2, 0 };
	vertices[1].position = { (float)length / 2,  height / 2, 0 };
	vertices[2].position = { (float)-length / 2,  height / 2, 0 };

	vertices[0].uv = DirectX::XMFLOAT2(0.5f, 0);
	vertices[1].uv = DirectX::XMFLOAT2(1, 1);
	vertices[2].uv = DirectX::XMFLOAT2(0, 1);

	for (int i = 0; i < 3; i++) {
		vertices[i].color = color;
	}

	std::vector<unsigned short> indices(3);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	CreateShape(vertices, indices, device);

	SetTransform(DirectX::XMMatrixTranslation(x, y, 0), DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());
}