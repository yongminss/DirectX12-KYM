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

// GameFramework를 사용하기 위해 필요한 (Device, CommandList, Object 등) 객체를 생성
void GameFramework::CreateGameFramework(HWND &hwnd)
{
	// 1. DirectX 12를 사용하기 위해 Device를 생성
	CreateDirectDevice();
	// 2. DirectX 12 그래픽 렌더링을 위해 CommandQueue와 CommandList를 생성
	CreateCommandQueueAndList();
	// 3. Double Buffering을 위해 SwapChain을 생성
	CreateSwapChain(hwnd);
	// 4. CPU - GPU 동기화를 위해 Fence를 생성
	CreateFence();
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

	// 사용이 끝난 COM 객체는 Release를 호출하여 갈비지 콜렉터가 삭제하도록 함
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

// Double Buffering을 하기 위해 SwapChain을 생성해야 함
void GameFramework::CreateSwapChain(HWND &hwnd)
{
	// SwapChain을 만들기 전에 다중 샘플링을 하기 위해 품질 지원을 검사
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSampleQualityLevel;
	MultiSampleQualityLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MultiSampleQualityLevel.SampleCount = 4; // MSAA (Multi-Sample Anti-Aliasing) 4x 다중 샘플링 - 4개의 서브 픽셀을 각각 샘플링하여 최종 색상 결정
	MultiSampleQualityLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MultiSampleQualityLevel.NumQualityLevels = 0;

	// Device가 지원하는 다중 샘플의 품질 확인
	m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSampleQualityLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

	// 품질 수준이 1보다 크면 다중 샘플링을 활성화
	unsigned int nMultiSampleQualityLevel = MultiSampleQualityLevel.NumQualityLevels;
	m_ActiveMSAA = (nMultiSampleQualityLevel > 1) ? true : false;

	// SwapChain 생성
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	// BufferDesc - 스왑 체인 버퍼의 성질을 설정하는 구조체 (ex. width, height, RefreshRate ... etc.)
	SwapChainDesc.BufferDesc.Width = 800;
	SwapChainDesc.BufferDesc.Height = 600;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // RefreshRate - 화면 갱신 횟수, 1초에 60Hz
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 후면 버퍼(픽셀)의 형식, 하나의 픽셀은 32bit로 설정
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 스캔 라인 그리기 모드 지정, 스캔 라인 순서를 지정하지 않음
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 모니터 해상도에 맞게 확대하는 방법, 스케일링 지정 x
	// SampleDesc - 다중 샘플링의 품질을 설정할 수 있는 구조체, 품질 검사에서 얻은 값으로 설정해야 함
	SwapChainDesc.SampleDesc.Count = (m_ActiveMSAA) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_ActiveMSAA) ? nMultiSampleQualityLevel - 1 : 0;
	SwapChainDesc.OutputWindow = hwnd; // 출력 될 windows 설정
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 설정, 렌더 타겟용으로 사용하도록 결정
	SwapChainDesc.BufferCount = 2; // 스왑 체인의 버퍼 개수, 전면 버퍼와 후면 버퍼를 사용
	SwapChainDesc.Windowed = true; // 창모드 설정 - true이면 창 모드, false이면 전체 모드
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스와핑을 처리하는 선택사항 지정, 버퍼를 유지하면 비용이 많이 발생하므로 버퍼 내용을 폐기하도록 설정
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 스왑 체인 동작에 대한 선택사항 지정, 응용 프로그램이 디스플레이 모드를 변경할 수 있도록 설정

	m_Factory->CreateSwapChain(m_CommandQueue, &SwapChainDesc, (IDXGISwapChain**)&m_SwapChain);
}

// CPU가 명령한 프레임 렌더링 데이터 작업이 GPU에서 끝나지 않았으면 CPU에서 새로운 명령을 넣지 못하게 막기 위해 Fence를 생성
void GameFramework::CreateFence()
{
	m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_Fence); // 0번 - FenceValue의 초기 값, 0으로 설정하여 0번 프레임부터 시작

	m_FenceEvent = CreateEvent(NULL, false, false, NULL);
}

// DirectX 12 게임을 플레이 할 수 있도록 매 프레임마다 반복 (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	// CommandList에서 렌더링을 하기 전에 Reset을 호출하여 CommandList를 Open 상태로 만들어야 Commands를 담을 수 있음
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	

	/* Rendering, Timer Reset 등의 작업 수행 */

	// 큐에 명령을 담기 위해 CommandList를 Close
	m_CommandList->Close();

	ID3D12CommandList *CommandList[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandList);
}