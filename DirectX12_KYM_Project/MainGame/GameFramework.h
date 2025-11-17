#pragma once

class Scene;

#define SWAP_CHAIN_COUNT 2

// DirectX 사용을 위한 장치 및 리소스 등을 생성하고 관리
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

	ID3D11Device *m_D3d11Device = nullptr;
	ID3D11DeviceContext* m_D3d11DeviceContext = nullptr;
	ID3D11On12Device *m_D3d11on12Device = nullptr;
	ID2D1Factory3 *m_D2dFactory = nullptr;
	IDWriteFactory *m_DwriteFactory = nullptr;
	ID2D1Device2 *m_D2dDevice = nullptr;
	ID2D1DeviceContext2 *m_D2dContext = nullptr;

	ID3D11Resource *m_WrappedBackBuffers[SWAP_CHAIN_COUNT]{};
	ID2D1Bitmap1 *m_D2dRenderTargets[SWAP_CHAIN_COUNT]{};

	IDWriteTextFormat *m_TextFormat = nullptr;
	ID2D1SolidColorBrush *m_TextBrush = nullptr;

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

	void CreateD3D11on12Device();
	void CreateD2DDevice();
	void CreateWrappedResource();
	void CreateTextResource();

	void CreateScene();

	void WaitToCompleteGpu();
	void MoveNextFrame();

	void GameFrameworkLoop();

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(UINT MessageIndex, LPARAM Lparam);
};