#include "stdafx.h"
#include "GameFramework.h"


GameFramework::GameFramework()
{

}

GameFramework::~GameFramework()
{

}

// GameFramework를 사용하기 위해 필요한 (Device, CommandList, Object 등) 객체를 생성
void GameFramework::CreateGameFramework()
{
	// 1. DirectX 12를 사용하기 위해 Device를 생성
	CreateDirectDevice();
	// 2. DirectX 12 그래픽 렌더링을 위해 CommandQueue와 CommandList를 생성
	CreateCommandQueueAndList();
}

// Direct3D를 사용하기 위해 장치를 생성 - DXGI
void GameFramework::CreateDirectDevice()
{
	unsigned int nFactoryFlag = 0;

	// DXGI 팩토리 생성 - 출력 담당, 어댑터의 생성 or 열거 등을 수행하는 역할, COM 객체
	CreateDXGIFactory2(nFactoryFlag, __uuidof(IDXGIFactory4), (void**)&m_Factory);

	// DXGI 팩토리를 생성했으니 그래픽 어댑터를 열거
	IDXGIAdapter1 *Adapter = nullptr;
	int GraphicIndex = 0;
	unsigned int HighPerformance = 0;

	// 성능이 가장 높은 그래픽 카드를 찾는 검사 수행
	for (int i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, &Adapter); ++i) {
		DXGI_ADAPTER_DESC1 AdapterDesc;
		Adapter->GetDesc1(&AdapterDesc);
		if (HighPerformance <= AdapterDesc.DedicatedVideoMemory) {
			GraphicIndex = i;
			HighPerformance = AdapterDesc.DedicatedVideoMemory;
		}
	}
	// 위의 검사에서 찾은 그래픽 카드를 연결 - DirectX 12를 지원하면 특성 레벨 12.0을 연결하고 그렇지 않으면 11.0을 연결
	m_Factory->EnumAdapters1(GraphicIndex, &Adapter);
	if (Adapter == nullptr) {
		m_Factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&Adapter);
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_Device);
	}
	else
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_Device);

	if (Adapter != nullptr) Adapter->Release();
}

// DirectX 12 그래픽 렌더링을 위해 CommandQueue와 CommandList를 생성해야 함 - COM 객체
void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 기본적인 CommandQueue
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // GPU가 직접 실행할 수 있는 CommandList

	m_Device->CreateCommandQueue(&CommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_CommandQueue);
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_CommandAllocator);
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&m_CommandList);

	m_CommandList->Close();
}

// DirectX 12 게임을 플레이 할 수 있도록 매 프레임마다 반복 (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	/* Rendering, Timer Reset 등의 작업 수행 */

	m_CommandList->Close();

	ID3D12CommandList *CommandList[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandList);
}