#include "Line.h"
#include "d3dx12.h"
#include "Debugger.h"

Line::Line(float x1, float y1, float x2, float y2, ID3D12Device* device) {
	vertices.emplace_back();
	vertices.emplace_back();

	vertices[0].position = DirectX::XMFLOAT3(0, 0, 0);
	vertices[0].color = DirectX::XMFLOAT3(1, 0, 0);
	vertices[0].uv = DirectX::XMFLOAT2(0, 0);

	vertices[1].position = DirectX::XMFLOAT3(x2 - x1, y2 - y1, 0);
	vertices[1].color = vertices[0].color;
	vertices[1].uv = DirectX::XMFLOAT2(1, 0);

	CreateVertexBufferView(device);
	CreateTransform(device);
	*world = DirectX::XMMatrixTranslation(x1, y1, 0);
}

Line::~Line() {

}

void Line::CreateVertexBufferView(ID3D12Device* device) {
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

void Line::CreateTransform(ID3D12Device* device) {
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

void Line::Draw(ID3D12GraphicsCommandList* cmdList) {
	cmdList->SetDescriptorHeaps(1, worldDescriptorHeap.GetAddressOf());
	cmdList->SetGraphicsRootDescriptorTable(1, worldDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	cmdList->DrawInstanced(2, 1, 0, 0);
}