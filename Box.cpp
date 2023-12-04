#include "Box.h"

Box::Box(int sx, int sy, int ex, int ey, ID3D12Device* device, DirectX::XMFLOAT3 color) {
	std::vector<VertexData> vertices(4);
	float halfX = (float)(ex - sx) / 2.0f;
	float halfY = (float)(ey - sy) / 2.0f;

	vertices[0].position = { -halfX,  halfY, 0 };
	vertices[1].position = {  halfX,  halfY, 0 };
	vertices[2].position = { -halfX, -halfY, 0 };
	vertices[3].position = {  halfX, -halfY, 0 };

	vertices[0].uv = DirectX::XMFLOAT2(0, 1);
	vertices[1].uv = DirectX::XMFLOAT2(1, 1);
	vertices[2].uv = DirectX::XMFLOAT2(0, 0);
	vertices[3].uv = DirectX::XMFLOAT2(1, 0);

	for (int i = 0; i < 4; i++) {
		vertices[i].color = color;
	}

	std::vector<unsigned short> indices(6);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;

	CreateShape(vertices, indices, device);

	SetTransform(DirectX::XMMatrixTranslation(sx, sy, 0), DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());
}