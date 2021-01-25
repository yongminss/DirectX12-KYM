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
	unsigned int nMultiSampleQualityLevel = MultiSampleQualityLevel.NumQualityLevels;
	m_ActiveMSAA = (nMultiSampleQualityLevel > 1) ? true : false;

	// SwapChain ����
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
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
	SwapChainDesc.SampleDesc.Quality = (m_ActiveMSAA) ? nMultiSampleQualityLevel - 1 : 0;
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

// DirectX 12 ������ �÷��� �� �� �ֵ��� �� �����Ӹ��� �ݺ� (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	// CommandList���� �������� �ϱ� ���� Reset�� ȣ���Ͽ� CommandList�� Open ���·� ������ Commands�� ���� �� ����
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	

	/* Rendering, Timer Reset ���� �۾� ���� */

	// ť�� ����� ��� ���� CommandList�� Close
	m_CommandList->Close();

	ID3D12CommandList *CommandList[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandList);
}