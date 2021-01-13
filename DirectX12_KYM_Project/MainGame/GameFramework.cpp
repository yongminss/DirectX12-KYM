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

	// DXGI ���丮 ���� - ��� ���, ������� ���� or ���� ���� �����ϴ� ����
	::CreateDXGIFactory2(nFactoryFlag, __uuidof(IDXGIFactory4), (void**)&m_Factory);

}