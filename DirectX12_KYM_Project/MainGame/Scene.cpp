#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_Player != nullptr) delete m_Player;
	if (m_Terrain != nullptr) delete m_Terrain;
	if (m_Skybox != nullptr) delete m_Skybox;
	if (m_UserInterface != nullptr) delete m_UserInterface;
	for (int i = 0; i < m_GameObjects.size(); ++i) if (m_GameObjects[i] != nullptr) delete m_GameObjects[i];
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_RANGE DescriptorRange[2];
	// Texture Count 1
	ZeroMemory(&DescriptorRange, sizeof(DescriptorRange));
	DescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[0].BaseShaderRegister = 0;
	DescriptorRange[0].RegisterSpace = 0;
	DescriptorRange[0].NumDescriptors = 1;
	DescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// Texture Count 2
	DescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[1].BaseShaderRegister = 1;
	DescriptorRange[1].RegisterSpace = 0;
	DescriptorRange[1].NumDescriptors = 2;
	DescriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameter[4];
	ZeroMemory(&RootParameter, sizeof(RootParameter));
	// Camera
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[0].Constants.ShaderRegister = 0;
	RootParameter[0].Constants.RegisterSpace = 0;
	RootParameter[0].Constants.Num32BitValues = 32;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// GameObject
	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[1].Constants.ShaderRegister = 1;
	RootParameter[1].Constants.RegisterSpace = 0;
	RootParameter[1].Constants.Num32BitValues = 16;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Texture
	RootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[2].DescriptorTable.pDescriptorRanges = &DescriptorRange[0];
	RootParameter[2].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Texture - Base + Detail
	RootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[3].DescriptorTable.pDescriptorRanges = &DescriptorRange[1];
	RootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc;
	ZeroMemory(&StaticSamplerDesc, sizeof(StaticSamplerDesc));
	StaticSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	StaticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDesc.MipLODBias = 0;
	StaticSamplerDesc.MaxAnisotropy = 1;
	StaticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	StaticSamplerDesc.MinLOD = 0;
	StaticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	StaticSamplerDesc.ShaderRegister = 0;
	StaticSamplerDesc.RegisterSpace = 0;
	StaticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력-조립(IA) 단계 허용
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = 4;
	RootSignatureDesc.pStaticSamplers = &StaticSamplerDesc;
	RootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob *RootSignatureBlob = nullptr;
	ID3DBlob *ErrorBlob = nullptr;

	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_RootSignature);

	if (RootSignatureBlob != nullptr) RootSignatureBlob->Release();
	if (ErrorBlob != nullptr) ErrorBlob->Release();
}

void Scene::CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// 리소스를 그래픽스 파이프라인에 연결하는 역할을 하는 RootSignature 생성
	CreateRootSignature(Device);

	// Camera를 가지고 있으며 플레이어가 직접 조종하는 오브젝트인 Player 생성
	m_Player = new Player();
	m_Player->CreateGameObject(Device, CommandList, m_RootSignature);

	// 각 정점 마다 높낮이가 다른 지형(Terrain) 생성
	m_Terrain = new Terrain();
	m_Terrain->CreateGameObject(Device, CommandList, m_RootSignature);
	m_Terrain->SetPosition(DirectX::XMFLOAT3(-1250.f, -150.f, -1250.f));

	// 게임의 배경 역할을 하는 Skybox 생성
	m_Skybox = new Skybox();
	m_Skybox->CreateGameObject(Device, CommandList, m_RootSignature);

	// 게임에 필요한 UI 생성
	m_UserInterface = new UserInterface();
	m_UserInterface->CreateGameObject(Device, CommandList, m_RootSignature);
	m_UserInterface->SetPosition(DirectX::XMFLOAT3(-0.75f, -0.75f, 0.f));

	// 게임 월드에 등장하는 Game Object 생성
	m_GameObjects.reserve(100);
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			m_GameObjects.emplace_back(new GameObject());
			m_GameObjects.back()->CreateGameObject(Device, CommandList, m_RootSignature);
			m_GameObjects.back()->SetPosition(DirectX::XMFLOAT3(-225.f + (i * 50), -50.f, 50.f + (50 * j)));
		}
	}
}

void Scene::Animate(float ElapsedTime)
{
	if (m_Player != nullptr) m_Player->Animate(ElapsedTime);
	if (m_Skybox != nullptr) m_Skybox->Animate(ElapsedTime, m_Player->GetPosition());
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	if (m_Player != nullptr) m_Player->Render(CommandList);
	if (m_Terrain != nullptr) m_Terrain->Render(CommandList);
	if (m_Skybox != nullptr) m_Skybox->Render(CommandList);
	if (m_UserInterface != nullptr) m_UserInterface->Render(CommandList);
	for (int i = 0; i < m_GameObjects.size(); ++i) if (m_GameObjects[i] != nullptr) m_GameObjects[i]->Render(CommandList);
}

void Scene::KeyboardMessage(UINT MessageIndex, WPARAM Wparam)
{
	switch (MessageIndex)
	{
	case WM_KEYDOWN:
		switch (Wparam)
		{
		case 'w':
		case 'W':
			m_Player->ActiveMove(0, true);
			break;

		case 's':
		case 'S':
			m_Player->ActiveMove(1, true);
			break;

		case 'a':
		case 'A':
			m_Player->ActiveMove(2, true);
			break;

		case 'd':
		case 'D':
			m_Player->ActiveMove(3, true);
			break;
		}
		break;

	case WM_KEYUP:
		switch (Wparam)
		{
		case 'w':
		case 'W':
			m_Player->ActiveMove(0, false);
			break;

		case 's':
		case 'S':
			m_Player->ActiveMove(1, false);
			break;

		case 'a':
		case 'A':
			m_Player->ActiveMove(2, false);
			break;

		case 'd':
		case 'D':
			m_Player->ActiveMove(3, false);
			break;
		}
		break;
	}
}

void Scene::MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam)
{
	switch (MessageIndex)
	{
	case WM_MOUSEMOVE:
	{
		SetCursor(NULL);
		POINT MousePos{};
		MousePos.x = LOWORD(Lparam);
		MousePos.y = HIWORD(Lparam);

		// 플레이어가 움직이는 마우스 좌표에 따라 플레이어 오브젝트가 회전
		if (MousePos.x > m_PreviousPos.x) {
			m_Player->Rotate(DirectX::XMFLOAT3(0.f, +1.5f, 0.f));
		}
		else {
			m_Player->Rotate(DirectX::XMFLOAT3(0.f, -1.5f, 0.f));
		}
		m_PreviousPos.x = MousePos.x;

		// 마우스 커서가 클라이언트 밖으로 벗어나지 않게 설정
		POINT NewMousePos = { Window_Width / 2, Window_Height / 2 };
		ClientToScreen(Hwnd, &NewMousePos);
		if (MousePos.x <= Window_Width / 3) SetCursorPos(NewMousePos.x, NewMousePos.y);
		if (MousePos.x >= Window_Width / 1.5) SetCursorPos(NewMousePos.x, NewMousePos.y);
	}
	break;

	break;
	}
}