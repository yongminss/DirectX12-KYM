#include "stdafx.h"
#include "GameFramework.h"


GameFramework::GameFramework()
{

}

GameFramework::~GameFramework()
{
	if (m_Factory != nullptr) m_Factory->Release();
	if (m_Device != nullptr) m_Device->Release();

	if (m_CommandQueue != nullptr) m_CommandQueue->Release();
	if (m_CommandAllocator != nullptr) m_CommandAllocator->Release();
	if (m_CommandList != nullptr) m_CommandList->Release();

	if (m_SwapChain != nullptr) m_SwapChain->Release();

	if (m_Fence != nullptr) m_Fence->Release();
	CloseHandle(m_FenceEvent);

	if (m_RenderTargetViewDescriptorHeap != nullptr) m_RenderTargetViewDescriptorHeap->Release();
	for (int i = 0; i < 2; ++i) if (m_RenderTargetBuffer[i] != nullptr) m_RenderTargetBuffer[i]->Release();
	if (m_DepthStencilViewDescriptorHeap != nullptr) m_DepthStencilViewDescriptorHeap->Release();
	if (m_DepthStencilBuffer != nullptr) m_DepthStencilBuffer->Release();
}

// GameFramework�� ����ϱ� ���� �ʿ��� (Device, CommandList, Object ��) ��ü�� ����
void GameFramework::CreateGameFramework(HWND &hwnd)
{
	// 1. DirectX 12�� ����ϱ� ���� Device�� ����
	CreateDirectDevice();
	// 2. DirectX 12 �׷��� �������� ���� CommandQueue�� CommandList�� ����
	CreateCommandQueueAndList();
	// 3. Double Buffering�� ���� SwapChain�� ����
	CreateSwapChain(hwnd);
	// 4. CPU - GPU ����ȭ�� ���� Fence�� ����
	CreateFence();
	// 5. ���ҽ�(Texture, Buffer)�� ����ϱ� ���� Descriptor Heap, Resource View, Resource�� ����
	CreateResource();
	// 6. ���� ������ ����Ǵ� (ex. Game Play or Rendering) ������ Scene�� ����
	CreateScene();
}

// Direct3D�� ����ϱ� ���� ��ġ�� ���� - DXGI
void GameFramework::CreateDirectDevice()
{
	unsigned int nFactoryFlag = 0;

	// DXGI ���丮 ���� - ��� ���, ������� ���� or ���� ���� �����ϴ� ����, COM ��ü
	CreateDXGIFactory2(nFactoryFlag, __uuidof(IDXGIFactory4), (void**)&m_Factory);

	// DXGI ���丮�� ���������� �׷��� ����͸� ����
	IDXGIAdapter1 *Adapter = nullptr;
	int GraphicIndex = 0;
	unsigned int HighPerformance = 0;

	// ������ ���� ���� �׷��� ī�带 ã�� �˻� ����
	for (int i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, &Adapter); ++i) {
		DXGI_ADAPTER_DESC1 AdapterDesc;
		ZeroMemory(&AdapterDesc, sizeof(DXGI_ADAPTER_DESC1));
		Adapter->GetDesc1(&AdapterDesc);
		if (HighPerformance <= AdapterDesc.DedicatedVideoMemory) {
			GraphicIndex = i;
			HighPerformance = AdapterDesc.DedicatedVideoMemory;
		}
	}
	// ���� �˻翡�� ã�� �׷��� ī�带 ���� - DirectX 12�� �����ϸ� Ư�� ���� 12.0�� �����ϰ� �׷��� ������ 11.0�� ����
	m_Factory->EnumAdapters1(GraphicIndex, &Adapter);
	if (Adapter == nullptr) {
		m_Factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&Adapter);
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_Device);
	}
	else
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_Device);

	// ����� ���� COM ��ü�� Release�� ȣ���Ͽ� ������ �ݷ��Ͱ� �����ϵ��� ��
	if (Adapter != nullptr) Adapter->Release();
}

// DirectX 12 �׷��� �������� ���� CommandQueue�� CommandList�� �����ؾ� �� - COM ��ü
void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // �⺻���� CommandQueue
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // GPU�� ���� ������ �� �ִ� CommandList

	m_Device->CreateCommandQueue(&CommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_CommandQueue);
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_CommandAllocator);
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&m_CommandList);

	m_CommandList->Close();
}

// Double Buffering�� �ϱ� ���� SwapChain�� �����ؾ� ��
void GameFramework::CreateSwapChain(HWND &hwnd)
{
	// SwapChain�� ����� ���� ���� ���ø��� �ϱ� ���� ǰ�� ������ �˻�
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevel;
	MultiSampleQualityLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MultiSampleQualityLevel.SampleCount = 4; // MSAA (Multi-Sample Anti-Aliasing) 4x ���� ���ø� - 4���� ���� �ȼ��� ���� ���ø��Ͽ� ���� ���� ����
	MultiSampleQualityLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MultiSampleQualityLevel.NumQualityLevels = 0;

	// Device�� �����ϴ� ���� ������ ǰ�� Ȯ��
	m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSampleQualityLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

	// ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ
	m_ActiveMSAA = (m_nMultiSampleQualityLevel > 1) ? true : false;
	m_nMultiSampleQualityLevel = MultiSampleQualityLevel.NumQualityLevels;

	// SwapChain ����
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	// BufferDesc - ���� ü�� ������ ������ �����ϴ� ����ü (ex. width, height, RefreshRate ... etc.)
	SwapChainDesc.BufferDesc.Width = 800;
	SwapChainDesc.BufferDesc.Height = 600;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // RefreshRate - ȭ�� ���� Ƚ��, 1�ʿ� 60Hz
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �ĸ� ����(�ȼ�)�� ����, �ϳ��� �ȼ��� 32bit�� ����
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // ��ĵ ���� �׸��� ��� ����, ��ĵ ���� ������ �������� ����
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // ����� �ػ󵵿� �°� Ȯ���ϴ� ���, �����ϸ� ���� x
	// SampleDesc - ���� ���ø��� ǰ���� ������ �� �ִ� ����ü, ǰ�� �˻翡�� ���� ������ �����ؾ� ��
	SwapChainDesc.SampleDesc.Count = (m_ActiveMSAA) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_ActiveMSAA) ? m_nMultiSampleQualityLevel - 1 : 0;
	SwapChainDesc.OutputWindow = hwnd; // ��� �� windows ����
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �ĸ� ���ۿ� ���� ǥ�� ��� ��İ� CPU�� ���� ��� ����, ���� Ÿ�ٿ����� ����ϵ��� ����
	SwapChainDesc.BufferCount = 2; // ���� ü���� ���� ����, ���� ���ۿ� �ĸ� ���۸� ���
	SwapChainDesc.Windowed = true; // â��� ���� - true�̸� â ���, false�̸� ��ü ���
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �������� ó���ϴ� ���û��� ����, ���۸� �����ϸ� ����� ���� �߻��ϹǷ� ���� ������ ����ϵ��� ����
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���� ü�� ���ۿ� ���� ���û��� ����, ���� ���α׷��� ���÷��� ��带 ������ �� �ֵ��� ����

	m_Factory->CreateSwapChain(m_CommandQueue, &SwapChainDesc, (IDXGISwapChain**)&m_SwapChain);
}

// CPU�� ����� ������ ������ ������ �۾��� GPU���� ������ �ʾ����� CPU���� ���ο� ����� ���� ���ϰ� ���� ���� Fence�� ����
void GameFramework::CreateFence()
{
	m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_Fence); // 0�� - FenceValue�� �ʱ� ��, 0���� �����Ͽ� 0�� �����Ӻ��� ����

	m_FenceEvent = CreateEvent(NULL, false, false, NULL);
}

// ���ҽ�(Texture, Buffer)�� ����� ���� Resource View�� �����ϰ� ���� �޸� �ּ��� �Ű�ü�� �� Descriptor Heap�� ����
void GameFramework::CreateResource()
{
	// RenderTarget View�� ���� Descriptor Heap�� ����
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.NumDescriptors = 2; // �������� ���� ���� - ����� �ĸ��� ����� ���̹Ƿ� 2
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;
	m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_RenderTargetViewDescriptorHeap);

	// Depth-Stencil View�� ���� Descriptor Heap�� ����
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescriptorHeapDesc.NumDescriptors = 1; // 1���� Depth-Stencil Buffer�� ���
	m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_DepthStencilViewDescriptorHeap);

	// RenderTarget View ���� - �̹� SwapChain�� ��������Ƿ� ������ ���ҽ��� �ʿ� ���� (View�� �����Ϸ��� ���ҽ��� �ʿ�)
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < 2; ++i) { // 2���� Buffer
		m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_RenderTargetBuffer[i]);
		m_Device->CreateRenderTargetView(m_RenderTargetBuffer[i], nullptr, RenderTargetDescriptorHandle); // nullptr - ���ҽ� ���İ� ���� �並 �������
		RenderTargetDescriptorHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Depth-Stencil Buffer ���� - RenderTarget View�� �޸� �̸� ������ ���ҽ��� �����Ƿ� ���ҽ��� �����ؾ� ��
	// CreateCommittedResource()�� ���ҽ��� ������� D3D12_HEAP_PROPERTIES, D3D12_RESOURCE_DESC, D3D12_CLEAR_VALUE ����ü�� �����ؾ� ��
	D3D12_HEAP_PROPERTIES HeapProperties;
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // Default - CPU�� ���� �Ұ�, GPU�� �а� ���Ⱑ ����, ���� ������ �����ϴ� �޸� ���� �������
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = 800;
	ResourceDesc.Height = 600;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = (m_ActiveMSAA) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (m_ActiveMSAA) ? m_nMultiSampleQualityLevel - 1 : 0;

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;

	m_Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void**)&m_DepthStencilBuffer);

	// ���ҽ��� ���������Ƿ� Depth-Stencil View ����
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle = m_DepthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, DepthStencilDescriptorHandle);
}

// ���� ������ ����Ǹ� RenderTarget�� Rendering�� �� ������Ʈ�� ������ �ִ� Scene�� ����
void GameFramework::CreateScene()
{
	m_Scene = new Scene();
	// ���ҽ��� �׷��Ƚ� ���������ο� �����ϱ� ���� RootSignature ����
	m_Scene->CreateRootSignature(m_Device);
	// �׷��Ƚ� ���������ΰ� HLSL�� �����ϱ� ���� Shader Ŭ���� ����
	m_Scene->CreateShader(m_Device);
}

// DirectX 12 ������ �÷��� �� �� �ֵ��� �� �����Ӹ��� �ݺ� (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	// CommandList���� �������� �ϱ� ���� Reset�� ȣ���Ͽ� CommandList�� Open ���·� ������ Commands�� ���� �� ����
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	// RenderTarget Buffer�� ���� Resource Barrier�� ����
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // ���ҽ� ��� ��ȭ�� ��Ÿ���� ���� �庮
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_RenderTargetBuffer[0];
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �ĸ� ���ۿ� write �� �� �ִ� ���·� ����
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	// RenderTarget & Depth-Stencil View�� CPU �ּҸ� Descriptor�� ���� ������
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle = m_DepthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// RenderTarget & Depth-Stencil�� ���ϴ� ������ �ʱ�ȭ
	float BackgroundColor[4] = { 0.2f, 0.f, 0.2f, 1.f };
	m_CommandList->ClearRenderTargetView(RenderTargetDescriptorHandle, BackgroundColor, 0, nullptr);
	m_CommandList->ClearDepthStencilView(DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	// RenderTarget View�� Depth-Stencil View�� Descriptor�� ���� ���-����(OM) �ܰ迡 ����
	m_CommandList->OMSetRenderTargets(1, &RenderTargetDescriptorHandle, true, &DepthStencilDescriptorHandle);

	// Rendering�� �ʿ��� ����� CommandList�� ���� ���������� Resource Barrier�� ���� ����
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // Present�� �Ͽ� read ���·� ����
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	// ť�� ����� ��� ���� CommandList�� Close
	m_CommandList->Close();

	ID3D12CommandList *CommandLists[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandLists);

	m_SwapChain->Present(0, 0);
}