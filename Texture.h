#pragma once

#include <d3d12.h>
#include "tool/DirectXTex2022/DirectXTex.h"

#include <wrl.h>

#include <string>
#include <memory>

class Texture
{
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	class Shape* shape;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	DirectX::XMFLOAT2 splitUV;
	std::vector<DirectX::XMFLOAT2> normalUV;
	DirectX::XMFLOAT2 splitNum;

	bool customShape;

public:
	Texture(std::wstring fileName, class Renderer* renderer, int x, int y, int splitX = 1, int splitY = 1, class Shape* customShape = nullptr);
	~Texture();

private:
	void CreateTexture(std::wstring fileName, class Renderer* renderer);

public:
	void Draw(ID3D12GraphicsCommandList* cmdList, int indexX = 0, int indexY = 0);
	void SetImageArray(int indexX, int indexY);

	class Shape* GetShape() { return shape; }

};

