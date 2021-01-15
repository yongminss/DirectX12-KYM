#pragma once
// DirectX Device 초기화 및 관리
class GameFramework
{
	IDXGIFactory4 *m_Factory = nullptr;
	ID3D12Device *m_Device = nullptr;

	ID3D12CommandQueue *m_CommandQueue = nullptr;
	ID3D12CommandAllocator *m_CommandAllocator = nullptr;
	ID3D12GraphicsCommandList *m_CommandList = nullptr;

public:
	GameFramework();
	~GameFramework();

	void CreateGameFramework();
	void CreateDirectDevice();
	void CreateCommandQueueAndList();

	void GameFrameworkLoop();
};
