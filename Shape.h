#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <vector>
#include <memory>

class Shape
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

	std::vector<unsigned short> indices;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> worldDescriptorHeap;
	DirectX::XMMATRIX* world;

	VertexData* verticesMap;

	DirectX::XMMATRIX position;
	DirectX::XMMATRIX rotate;
	DirectX::XMMATRIX scale;

public:
	Shape();
	~Shape();

private:
	void CreateVertexBufferView(ID3D12Device* device);
	void CreateIndexBufferView(ID3D12Device* device);
	void CreateTransform(ID3D12Device* device);

public:
	void CreateShape(std::vector<VertexData> vertex, std::vector<unsigned short> index, ID3D12Device* device);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotate);
	void SetScale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetPosition(DirectX::XMMATRIX transform);
	DirectX::XMFLOAT3 GetRotation(DirectX::XMMATRIX transform);
	DirectX::XMFLOAT3 GetScale(DirectX::XMMATRIX transform);

	void SetTransform(DirectX::XMMATRIX position, DirectX::XMMATRIX rotation, DirectX::XMMATRIX scale);
	void SetUV(std::vector<DirectX::XMFLOAT2> uv);

	DirectX::XMMATRIX GetTransform() { return *world; }

	std::vector<DirectX::XMFLOAT2> GetUV();
};