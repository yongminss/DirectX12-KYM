#pragma once

class Scene;

#define SWAP_CHAIN_COUNT 2

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
	UINT64 m_FenceValueCount[SWAP_CHAIN_COUNT]{};
	HANDLE m_FenceEvent{};

	ID3D12DescriptorHeap *m_RenderTargetViewDescriptorHeap = nullptr;
	ID3D12Resource *m_RenderTargetBuffer[SWAP_CHAIN_COUNT]{};
	ID3D12DescriptorHeap *m_DepthStencilViewDescriptorHeap = nullptr;
	ID3D12Resource *m_DepthStencilBuffer = nullptr;

	Scene *m_Scene = nullptr;

	HWND m_Hwnd{};

	bool m_ActiveMsaa = false;
	unsigned int m_MultiSampleQualityLevel = 0;

	unsigned int m_SwapChainIndex = 0;

	unsigned int m_FrameRate = 0;
	float m_ElapsedTime = 0.f;
	float m_SecondsCounter = 0.f;

public:
	GameFramework();
	~GameFramework();

	void CreateGameFramework(HWND &Hwnd);

	void CreateDirectDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateFence();
	void CreateResource();
	void CreateScene();

	void WaitToCompleteGpu();
	void MoveNextFrame();

	void GameFrameworkLoop();

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(UINT MessageIndex, LPARAM Lparam);
};