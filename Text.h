#pragma once

#include "SpriteFont.h"
#include "ResourceUploadBatch.h"
#include <wrl.h>

#include <string>

class Text
{
private:
	DirectX::SpriteFont* spriteFont = nullptr;
	DirectX::SpriteBatch* spriteBatch = nullptr;

	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

public:
	Text(ID3D12Device* device, ID3D12CommandQueue* commandQueue, D3D12_VIEWPORT viewPort, class Renderer* renderer, std::wstring fontFileName);

	void Draw(ID3D12GraphicsCommandList* commandList, std::wstring text, DirectX::XMFLOAT2 pos, DirectX::XMVECTOR color);
};

