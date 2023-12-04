#include "Circle.h"

Circle::Circle(int x, int y, int r, ID3D12Device* device, DirectX::XMFLOAT3 color) {
	std::vector<VertexData> vertices(362);

	vertices[0].position.x = 0;
	vertices[0].position.y = 0;
	vertices[0].position.z = 0.0f;
	vertices[0].color = color;
	vertices[0].uv = DirectX::XMFLOAT2(0.5f, 0.5f);

	for (float i = 1; i < 361; i++) {
		float sin = DirectX::XMScalarSin(DirectX::XMConvertToRadians(i));
		float cos = DirectX::XMScalarCos(DirectX::XMConvertToRadians(i));

		vertices[i].position.x = cos * (float)r;
		vertices[i].position.y = sin * (float)r;
		vertices[i].position.z = 0.0f;
		vertices[i].color = color;
		vertices[i].uv = DirectX::XMFLOAT2((cos + 1.0f) / 2.0f, ((sin + 1.0f) / 2.0f));
	}
	vertices[361] = vertices[1];

	std::vector<unsigned short> indices(1080);
	for (int i = 0; i < 359; i++) {
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}

	indices[359 * 3] = 0;
	indices[359 * 3 + 1] = 360;
	indices[359 * 3 + 2] = 1;

	CreateShape(vertices, indices, device);

	SetTransform(DirectX::XMMatrixTranslation(x, y, 0), DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());
}