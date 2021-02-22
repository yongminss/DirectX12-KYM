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
	CloseHandle(m_FenceEvent);

	if (m_RenderTargetViewDescriptorHeap != nullptr) m_RenderTargetViewDescriptorHeap->Release();
	for (int i = 0; i < 2; ++i) if (m_RenderTargetBuffer[i] != nullptr) m_RenderTargetBuffer[i]->Release();
	if (m_DepthStencilViewDescriptorHeap != nullptr) m_DepthStencilViewDescriptorHeap->Release();
	if (m_DepthStencilBuffer != nullptr) m_DepthStencilBuffer->Release();
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
	// 5. 리소스(Texture, Buffer)를 사용하기 위해 Descriptor Heap, Resource View, Resource를 생성
	CreateResource();
	// 6. 실제 게임이 진행되는 (ex. Game Play or Rendering) 영역인 Scene을 생성
	CreateScene();
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
		ZeroMemory(&AdapterDesc, sizeof(DXGI_ADAPTER_DESC1));
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
	ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
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
	m_ActiveMSAA = (m_nMultiSampleQualityLevel > 1) ? true : false;
	m_nMultiSampleQualityLevel = MultiSampleQualityLevel.NumQualityLevels;

	// SwapChain 생성
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
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
	SwapChainDesc.SampleDesc.Quality = (m_ActiveMSAA) ? m_nMultiSampleQualityLevel - 1 : 0;
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

// 리소스(Texture, Buffer)를 만들기 위해 Resource View를 생성하고 가상 메모리 주소의 매개체로 쓸 Descriptor Heap을 생성
void GameFramework::CreateResource()
{
	// RenderTarget View를 위한 Descriptor Heap을 생성
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.NumDescriptors = 2; // 서술자의 개수 결정 - 전면과 후면을 사용할 것이므로 2
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;
	m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_RenderTargetViewDescriptorHeap);

	// Depth-Stencil View를 위한 Descriptor Heap을 생성
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescriptorHeapDesc.NumDescriptors = 1; // 1개의 Depth-Stencil Buffer만 사용
	m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_DepthStencilViewDescriptorHeap);

	// RenderTarget View 생성 - 이미 SwapChain을 만들었으므로 별도의 리소스가 필요 없음 (View를 생성하려면 리소스가 필요)
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < 2; ++i) { // 2개의 Buffer
		m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_RenderTargetBuffer[i]);
		m_Device->CreateRenderTargetView(m_RenderTargetBuffer[i], nullptr, RenderTargetDescriptorHandle); // nullptr - 리소스 형식과 같은 뷰를 만들어줌
		RenderTargetDescriptorHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Depth-Stencil Buffer 생성 - RenderTarget View와 달리 미리 만들어둔 리소스가 없으므로 리소스를 생성해야 함
	// CreateCommittedResource()로 리소스를 만드려면 D3D12_HEAP_PROPERTIES, D3D12_RESOURCE_DESC, D3D12_CLEAR_VALUE 구조체를 설정해야 함
	D3D12_HEAP_PROPERTIES HeapProperties;
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // Default - CPU는 접근 불가, GPU는 읽고 쓰기가 가능, 가장 빠르게 동작하는 메모리 힙에 만들어짐
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = 800;
	ResourceDesc.Height = 600;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = (m_ActiveMSAA) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (m_ActiveMSAA) ? m_nMultiSampleQualityLevel - 1 : 0;

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;

	m_Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void**)&m_DepthStencilBuffer);

	// 리소스를 생성했으므로 Depth-Stencil View 생성
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle = m_DepthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, DepthStencilDescriptorHandle);
}

// 실제 게임이 진행되며 RenderTarget에 Rendering을 할 오브젝트를 가지고 있는 Scene을 생성
void GameFramework::CreateScene()
{
	m_Scene = new Scene();
	// 리소스를 그래픽스 파이프라인에 연결하기 위해 RootSignature 생성
	m_Scene->CreateRootSignature(m_Device);
	// 그래픽스 파이프라인과 HLSL을 연결하기 위해 Shader 클래스 생성
	m_Scene->CreateShader(m_Device);
}

// DirectX 12 게임을 플레이 할 수 있도록 매 프레임마다 반복 (ex. CommandList Reset, Rendering, Timer Reset ... etc.)
void GameFramework::GameFrameworkLoop()
{
	// CommandList에서 렌더링을 하기 전에 Reset을 호출하여 CommandList를 Open 상태로 만들어야 Commands를 담을 수 있음
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, nullptr);

	// RenderTarget Buffer에 대한 Resource Barrier를 설정
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 리소스 사용 변화를 나타내는 전이 장벽
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_RenderTargetBuffer[0];
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 후면 버퍼에 write 할 수 있는 상태로 변경
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	// RenderTarget & Depth-Stencil View의 CPU 주소를 Descriptor를 통해 가져옴
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle = m_DepthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// RenderTarget & Depth-Stencil를 원하는 값으로 초기화
	float BackgroundColor[4] = { 0.2f, 0.f, 0.2f, 1.f };
	m_CommandList->ClearRenderTargetView(RenderTargetDescriptorHandle, BackgroundColor, 0, nullptr);
	m_CommandList->ClearDepthStencilView(DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	// RenderTarget View와 Depth-Stencil View를 Descriptor를 통해 출력-병합(OM) 단계에 연결
	m_CommandList->OMSetRenderTargets(1, &RenderTargetDescriptorHandle, true, &DepthStencilDescriptorHandle);

	// Rendering에 필요한 명령을 CommandList에 전부 삽입했으니 Resource Barrier의 상태 변경
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // Present를 하여 read 상태로 변경
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	// 큐에 명령을 담기 위해 CommandList를 Close
	m_CommandList->Close();

	ID3D12CommandList *CommandLists[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(1, CommandLists);

	m_SwapChain->Present(0, 0);
}