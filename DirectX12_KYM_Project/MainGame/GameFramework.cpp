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
	D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_Device);
}