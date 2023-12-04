#include "Shape.h"

#include "tool/d3dx12.h"

#include "Debugger.h"

Shape::Shape() {
	
}

Shape::~Shape() {
	if (vertexBuffer != nullptr) {
		vertexBuffer->Unmap(0, nullptr);
	}

	if (constBuffer != nullptr) {
		constBuffer->Unmap(0, nullptr);
	}
}

void Shape::CreateShape(std::vector<VertexData> vertex, std::vector<unsigned short> index, ID3D12Device* device) {
	vertices = vertex;
	indices = index;

	CreateVertexBufferView(device);
	CreateIndexBufferView(device);
	CreateTransform(device);
}

void Shape::CreateVertexBufferView(ID3D12Device* device) {
	auto size = sizeof(VertexData) * vertices.size();

	D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	Debugger::ErrorCheck(device->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexBuffer.ReleaseAndGetAddressOf())));

	verticesMap = nullptr;
	Debugger::ErrorCheck(vertexBuffer->Map(0, nullptr, (void**)&verticesMap));
	std::copy(std::begin(vertices), std::end(vertices), verticesMap);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = size;
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Shape::CreateIndexBufferView(ID3D12Device* device) {
	auto size = sizeof(unsigned short) * indices.size();

	D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	Debugger::ErrorCheck(device->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(indexBuffer.ReleaseAndGetAddressOf())));

	unsigned short* indicesMap = nullptr;
	Debugger::ErrorCheck(indexBuffer->Map(0, nullptr, (void**)&indicesMap));
	std::copy(std::begin(indices), std::end(indices), indicesMap);
	indexBuffer->Unmap(0, nullptr);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	indexBufferView.SizeInBytes = size;
}

void Shape::CreateTransform(ID3D12Device* device) {
	D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(DirectX::XMMATRIX) + 0xff) & ~0xff);

	Debugger::ErrorCheck(device->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constBuffer.ReleaseAndGetAddressOf())));

	Debugger::ErrorCheck(constBuffer->Map(0, nullptr, (void**)&world));
	*world = DirectX::XMMatrixIdentity();

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	Debugger::ErrorCheck(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(worldDescriptorHeap.ReleaseAndGetAddressOf())));

	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
	constantBufferViewDesc.BufferLocation = constBuffer->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = constBuffer->GetDesc().Width;

	device->CreateConstantBufferView(&constantBufferViewDesc, worldDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void Shape::Draw(ID3D12GraphicsCommandList* cmdList) {
	*world = scale * rotate * position;

	cmdList->SetDescriptorHeaps(1, worldDescriptorHeap.GetAddressOf());
	cmdList->SetGraphicsRootDescriptorTable(1, worldDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	cmdList->IASetIndexBuffer(&indexBufferView);
	cmdList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}

void Shape::SetTransform(DirectX::XMMATRIX position, DirectX::XMMATRIX rotation, DirectX::XMMATRIX scale) {
	this->position = position;
	rotate = rotation;
	this->scale = scale;
}

void Shape::SetUV(std::vector<DirectX::XMFLOAT2> uv) {
	for (int i = 0; i < vertices.size(); i++) {
		verticesMap[i].uv.x = uv[i].x;
		verticesMap[i].uv.y = uv[i].y;
	}
}

std::vector<DirectX::XMFLOAT2> Shape::GetUV() {
	std::vector<DirectX::XMFLOAT2> result(vertices.size());

	for (int i = 0; i < vertices.size(); i++) {
		result[i] = vertices[i].uv;
	}

	return result;
}

void Shape::SetPosition(DirectX::XMFLOAT3 pos) {
	position = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void Shape::SetRotation(DirectX::XMFLOAT3 rot) {
	rotate = DirectX::XMMatrixRotationX(rot.x);
	rotate = DirectX::XMMatrixRotationY(rot.y);
	rotate = DirectX::XMMatrixRotationZ(rot.z);
}

void Shape::SetScale(DirectX::XMFLOAT3 sca) {
	scale = DirectX::XMMatrixScaling(sca.x, sca.y, sca.z);
}

DirectX::XMFLOAT3 Shape::GetPosition(DirectX::XMMATRIX transform) {
	DirectX::XMFLOAT3 translation;
	DirectX::XMStoreFloat3(&translation, transform.r[3]);
	return translation;
}

DirectX::XMFLOAT3 Shape::GetRotation(DirectX::XMMATRIX transform) {
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationMatrix(transform));
	// Quaternion‚©‚çƒIƒCƒ‰[Šp‚É•ÏŠ·
	DirectX::XMFLOAT3 rotation;
	rotation.x = atan2f(2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x),
		quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
	rotation.y = asinf(-2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y));
	rotation.z = atan2f(2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z),
		quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z);

	return rotation;
}

DirectX::XMFLOAT3 Shape::GetScale(DirectX::XMMATRIX transform) {
	DirectX::XMFLOAT3 scale;
	scale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(transform.r[0]));
	scale.y = DirectX::XMVectorGetY(DirectX::XMVector3Length(transform.r[1]));
	scale.z = DirectX::XMVectorGetZ(DirectX::XMVector3Length(transform.r[2]));

	return scale;
}