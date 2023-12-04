#include "Renderer.h"
#include "Debugger.h"

#include <_d3dx12.h>

#include <string>

using Microsoft::WRL::ComPtr;

Renderer::Renderer(int width, int height, HWND hwnd) {
	Debugger::ErrorCheck(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
	
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugLayer = nullptr;
	Debugger::ErrorCheck(D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf())));
	debugLayer->EnableDebugLayer();

	Debugger::ErrorCheck(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())));
#else
	Debugger::ErrorCheck(CreateDXGIFactory1(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())));
#endif

	CreateDevice();
	CreateCommand();
	CreateSwapChain(width, height, hwnd);
	CreateRootSignature();
	CreateGraphicsPipeline();
	CreateRenderTarget();

	fenceVal = 0;
	Debugger::ErrorCheck(device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())));

	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = width;
	scissorRect.bottom = height;

	{
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーから見えるようにする
		descHeapDesc.NodeMask = 0;										// マスクは0
		descHeapDesc.NumDescriptors = 1;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// シェーダーリソースビュー用
		Debugger::ErrorCheck(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap)));

		DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
		matrix.r[0].m128_f32[0] = 2.0f / (float)width;
		matrix.r[1].m128_f32[1] = -2.0f / (float)height;
		matrix.r[3].m128_f32[0] = -1.0f;
		matrix.r[3].m128_f32[1] = 1.0f;

		D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(matrix) + 0xff) & ~0xff);

		Debugger::ErrorCheck(device->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constBuffer.ReleaseAndGetAddressOf())));

		DirectX::XMMATRIX* mapMatrix;
		Debugger::ErrorCheck(constBuffer->Map(0, nullptr, (void**)&mapMatrix));
		*mapMatrix = matrix;
		constBuffer->Unmap(0, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constBuffer->GetDesc().Width;

		device->CreateConstantBufferView(&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
	}

	graphicsMemory = std::make_unique<DirectX::GraphicsMemory>(device.Get());
}

Renderer::~Renderer() {
	RunCommand();

	CoUninitialize();
}

void Renderer::CreateDevice() {
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	if (factory.Get() != nullptr) {
		for (int i = 0; factory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
			adapters.push_back(tmpAdapter);
		}

		for (auto adapter : adapters) {
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring str = desc.Description;

			if (str.find(L"NVIDIA") != std::string::npos) {
				tmpAdapter = adapter;
				break;
			}
		}
	}

	HRESULT result;
	for (auto level : levels) {
		result = D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(device.ReleaseAndGetAddressOf()));
		if (SUCCEEDED(result)) {
			break;
		}
	}

	Debugger::ErrorCheck(result);
}

void Renderer::CreateCommand() {
	Debugger::ErrorCheck(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator.ReleaseAndGetAddressOf())));

	Debugger::ErrorCheck(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf())));

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Debugger::ErrorCheck(device->CreateCommandQueue(&desc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf())));
}

void Renderer::CreateSwapChain(int width, int height, HWND hwnd) {
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = false;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	desc.BufferCount = 2;

	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	Debugger::ErrorCheck(factory->CreateSwapChainForHwnd(cmdQueue.Get(), hwnd, &desc, nullptr, nullptr, (IDXGISwapChain1**)swapchain.ReleaseAndGetAddressOf()));
}

void Renderer::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE range[3];
	range[0] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// 座標変換行列
	range[1] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	// 座標行列
	range[2] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// テクスチャ

	D3D12_ROOT_PARAMETER rootParam[3] = {};
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam[0].DescriptorTable.pDescriptorRanges = &range[0];
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;

	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam[1].DescriptorTable.pDescriptorRanges = &range[1];
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;

	rootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParam[2].DescriptorTable.pDescriptorRanges = &range[2];
	rootParam[2].DescriptorTable.NumDescriptorRanges = 1;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.pParameters = rootParam;
	desc.NumParameters = 3;
	desc.pStaticSamplers = &samplerDesc;
	desc.NumStaticSamplers = 1;

	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> error;

	Debugger::ErrorCheck(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSigBlob.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	Debugger::ErrorCheck(device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf())));
}

void Renderer::CreateGraphicsPipeline() {
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> vs;
	ComPtr<ID3DBlob> ps;
	ComPtr<ID3DBlob> texturePS;

	Debugger::ErrorCheck(D3DCompileFromFile(L"BasicVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vs.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	Debugger::ErrorCheck(D3DCompileFromFile(L"TexPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "TexturePS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, texturePS.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	D3D12_RENDER_TARGET_BLEND_DESC blend = {};
	blend.BlendEnable = false;
	blend.LogicOpEnable = false;
	blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = rootSignature.Get();
	desc.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
	desc.PS = CD3DX12_SHADER_BYTECODE(texturePS.Get());

	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	desc.RasterizerState.MultisampleEnable = false;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.DepthClipEnable = true;

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.BlendState.IndependentBlendEnable = false;
	desc.BlendState.RenderTarget[0] = blend;

	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = _countof(inputLayout);

	desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	Debugger::ErrorCheck(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(texturePipeline.ReleaseAndGetAddressOf())));

	Debugger::ErrorCheck(D3DCompileFromFile(L"BasicPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, ps.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	desc.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
	Debugger::ErrorCheck(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipeline.ReleaseAndGetAddressOf())));
}

void Renderer::CreateRenderTarget() {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	Debugger::ErrorCheck(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps.ReleaseAndGetAddressOf())));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	Debugger::ErrorCheck(swapchain->GetDesc(&swcDesc));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	backBuffer.resize(swcDesc.BufferCount);
	auto handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < swcDesc.BufferCount; i++) {
		Debugger::ErrorCheck(swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffer[i].ReleaseAndGetAddressOf())));
		device->CreateRenderTargetView(backBuffer[i].Get(), &rtvDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void Renderer::BeginDraw() {
	auto index = swapchain->GetCurrentBackBufferIndex();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer[index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	cmdList->RSSetViewports(1, &viewPort);
	cmdList->RSSetScissorRects(1, &scissorRect);

	cmdList->SetPipelineState(pipeline.Get());
	//cmdList->SetPipelineState(texturePipeline.Get());

	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	cmdList->SetDescriptorHeaps(1, basicDescHeap.GetAddressOf());

	cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());
}

void Renderer::EndDraw() {

	auto index = swapchain->GetCurrentBackBufferIndex();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer[index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList->ResourceBarrier(1, &barrier);

	RunCommand();

	swapchain->Present(1, 0);

	graphicsMemory->Commit(cmdQueue.Get());
}

void Renderer::RunCommand() {
	cmdList->Close();

	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	while (fence->GetCompletedValue() != fenceVal) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	cmdAllocator->Reset();
	cmdList->Reset(cmdAllocator.Get(), nullptr);
}

void Renderer::SetNormalPipeline() {
	cmdList->SetPipelineState(pipeline.Get());
}

void Renderer::SetTexturePipeline() {
	cmdList->SetPipelineState(texturePipeline.Get());
}