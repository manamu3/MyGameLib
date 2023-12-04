#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "tool/DirectXTK2022/GraphicsMemory.h"

#include <vector>

class Renderer
{
private:
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> factory = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffer;
	UINT64 fenceVal;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> texturePipeline;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap = nullptr;

	std::unique_ptr<DirectX::GraphicsMemory> graphicsMemory = nullptr;

public:
	Renderer(int width, int height, HWND hwnd);
	~Renderer();

private:
	void CreateDevice();
	void CreateCommand();
	void CreateSwapChain(int width, int height, HWND hWnd);
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void CreateRenderTarget();

public:
	void BeginDraw();
	void EndDraw();
	void RunCommand();

	void SetNormalPipeline();
	void SetTexturePipeline();

public:
	ID3D12Device* GetDevice() { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }
	ID3D12CommandQueue* GetCommandQueue() { return cmdQueue.Get(); }
	D3D12_VIEWPORT GetViewPort() { return viewPort; }
};

