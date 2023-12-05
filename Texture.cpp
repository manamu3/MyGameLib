#include "Texture.h"
#include "DirectXTex.h"
#include "Debugger.h"
#include <vector>
#include "d3dx12.h"
#include "Renderer.h"
#include "Box.h"

using Microsoft::WRL::ComPtr;

Texture::Texture(std::wstring fileName, Renderer* renderer, int x, int y, int splitX, int splitY, Shape* customShape) {
	CreateTexture(fileName, renderer);

	splitUV = { 1.0f, 1.0f };
	splitNum = { (float)splitX, (float)splitY };
	this->customShape = customShape != nullptr;

	if (customShape == nullptr) {
		splitUV.x = 1.0f / (float)splitX;
		splitUV.y = 1.0f / (float)splitY;

		auto image = scratchImage.GetImage(0, 0, 0);
		DirectX::XMFLOAT2 imageScale = { image->width * splitUV.x, image->height * splitUV.y };

		shape = new Box(x, y, x + imageScale.x, y + imageScale.y, renderer->GetDevice());
	}
	else {
		shape = customShape;
	}

	normalUV = shape->GetUV();

	SetImageArray(1, 0);
}

Texture::~Texture() {
	if (!customShape) {
		delete shape;
		shape = nullptr;
	}
}

void Texture::CreateTexture(std::wstring fileName, Renderer* renderer) {
	Debugger::ErrorCheck(DirectX::LoadFromWICFile(fileName.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImage));

	auto image = scratchImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = image->slicePitch;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> textureUpload;
	Debugger::ErrorCheck(renderer->GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(textureUpload.ReleaseAndGetAddressOf())));

	UINT8* dataBegin;
	textureUpload->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));

	memcpy(dataBegin, image->pixels, image->slicePitch);

	textureUpload->Unmap(0, nullptr);

	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	desc.Width = static_cast<UINT64>(metadata.width);
	desc.Height = static_cast<UINT>(metadata.height);
	desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	desc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	desc.Format = metadata.format;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	Debugger::ErrorCheck(renderer->GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(texbuff.ReleaseAndGetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Debugger::ErrorCheck(renderer->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(basicDescHeap.ReleaseAndGetAddressOf())));

	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = metadata.format;
	shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);

	renderer->GetDevice()->CreateShaderResourceView(texbuff.Get(), &shaderResourceViewDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
	basicDescHeap->SetName(L"texture");

	D3D12_TEXTURE_COPY_LOCATION dest;
	dest.pResource = texbuff.Get();
	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;
	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = textureUpload.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Format = desc.Format;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(desc.Width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(desc.Height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(desc.DepthOrArraySize);
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(image->rowPitch);

	renderer->GetCommandList()->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texbuff.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	renderer->GetCommandList()->ResourceBarrier(1, &barrier);

	renderer->RunCommand();
}

void Texture::Draw(ID3D12GraphicsCommandList* cmdList, int indexX, int indexY) {
	cmdList->SetDescriptorHeaps(1, basicDescHeap.GetAddressOf());
	cmdList->SetGraphicsRootDescriptorTable(2, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	shape->Draw(cmdList);
}

void Texture::SetImageArray(int indexX, int indexY) {
	float x = (((float)indexX + 1.0f) / splitNum.x) - ((float)indexX / splitNum.x);
	float y = (((float)indexY + 1.0f) / splitNum.y) - ((float)indexY / splitNum.y);

	std::vector<DirectX::XMFLOAT2> uv(normalUV.size());
	for (int i = 0; i < normalUV.size(); i++) {
		uv[i].x = normalUV[i].x * x + x * (float)indexX;
		uv[i].y = normalUV[i].y * y + y * (float)indexY;
	}
	shape->SetUV(uv);
}