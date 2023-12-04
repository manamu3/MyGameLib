#include "Text.h"
#include "Renderer.h"

Text::Text(ID3D12Device* device, ID3D12CommandQueue* commandQueue, D3D12_VIEWPORT viewPort, Renderer* renderer, std::wstring fontFileName) {
	DirectX::ResourceUploadBatch resUploadBatch(device);
	resUploadBatch.Begin();
	DirectX::RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
	DirectX::SpriteBatchPipelineStateDescription pipelineStateDescription(rtState);
	spriteBatch = new DirectX::SpriteBatch(device, resUploadBatch, pipelineStateDescription);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap.ReleaseAndGetAddressOf()));

	spriteFont = new DirectX::SpriteFont(device, resUploadBatch, fontFileName.c_str(),
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorHeap->GetGPUDescriptorHandleForHeapStart());

	auto future = resUploadBatch.End(commandQueue);
	renderer->RunCommand();
	future.wait();
	spriteBatch->SetViewport(viewPort);
}

void Text::Draw(ID3D12GraphicsCommandList* commandList, std::wstring text, DirectX::XMFLOAT2 pos, DirectX::XMVECTOR color) {
	commandList->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());
	spriteBatch->Begin(commandList);
	spriteFont->DrawString(spriteBatch, text.c_str(), pos, color);
	spriteBatch->End();
}