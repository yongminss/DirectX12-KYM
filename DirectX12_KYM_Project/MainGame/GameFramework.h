#pragma once
// DirectX Device 초기화 및 관리
class GameFramework
{
private:
	IDXGIFactory4 *m_Factory = nullptr;
	ID3D12Device *m_Device = nullptr;

	ID3D12CommandQueue *m_CommandQueue = nullptr;
	ID3D12CommandAllocator *m_CommandAllocator = nullptr;
	ID3D12GraphicsCommandList *m_CommandList = nullptr;

	IDXGISwapChain3 *m_SwapChain = nullptr;

	ID3D12Fence *m_Fence = nullptr;

	bool m_ActiveMSAA = false;

public:
	GameFramework();
	~GameFramework();

	void CreateGameFramework(HWND &hwnd);

	void CreateDirectDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain(HWND &hwnd);
	void CreateFence();

	void GameFrameworkLoop();
};
