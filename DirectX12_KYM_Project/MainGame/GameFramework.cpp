#include "stdafx.h"
#include "GameFramework.h"


GameFramework::GameFramework()
{

}

GameFramework::~GameFramework()
{

}

// GameFramework�� ����ϱ� ���� �ʿ��� (Device, CommandList, Object ��) ��ü�� ����
void GameFramework::CreateGameFramework()
{
	// 1. DirectX 12�� ����ϱ� ���� Device�� ����
	CreateDirectDevice();
	// 2. DirectX 12 �׷��� �������� ���� CommandQueue�� CommandList�� ����
	CreateCommandQueueAndList();
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

// DirectX 12 ������ �÷��� �� �� �ֵ��� �� �����Ӹ��� �ݺ� (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	/* Rendering, Timer Reset ���� �۾� ���� */

	m_CommandList->Close();

	ID3D12CommandList *CommandList[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandList);
}