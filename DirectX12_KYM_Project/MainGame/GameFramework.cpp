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
}

// Direct3D를 사용하기 위해 장치를 생성 - DXGI
void GameFramework::CreateDirectDevice()
{
	unsigned int nFactoryFlag = 0;

	// DXGI 팩토리 생성 - 출력 담당, 어댑터의 생성 or 열거 등을 수행하는 역할
	::CreateDXGIFactory2(nFactoryFlag, __uuidof(IDXGIFactory4), (void**)&m_Factory);

}