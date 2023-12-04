#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>

class Line
{
public:
	typedef struct VertexData {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT2 uv;
	};

private:
	std::vector<VertexData> vertices;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> worldDescriptorHeap;
	DirectX::XMMATRIX* world;

	VertexData* verticesMap;

	DirectX::XMMATRIX position;
	DirectX::XMMATRIX rotate;
	DirectX::XMMATRIX scale;

public:
	Line(float x1, float y1, float x2, float y2, ID3D12Device* device);
	~Line();

private:
	void CreateVertexBufferView(ID3D12Device* device);
	void CreateTransform(ID3D12Device* device);

public:
	void Draw(ID3D12GraphicsCommandList* cmdList);
};