#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_Player != nullptr) delete m_Player;
	//if (m_GameObject != nullptr) delete m_GameObject;
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_ROOT_PARAMETER RootParameter[2];
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

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력-조립(IA) 단계 허용
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = 2;
	RootSignatureDesc.pStaticSamplers = nullptr;
	RootSignatureDesc.NumStaticSamplers = 0;

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
	m_Player->CreatePlayer(Device, CommandList, m_RootSignature);

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

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	if (m_Player != nullptr) m_Player->Render(CommandList);

	for (int i = 0; i < m_GameObjects.size(); ++i) if (m_GameObjects[i] != nullptr) m_GameObjects[i]->Render(CommandList);
}

void Scene::KeyboardMessage(UINT MessageIndex, WPARAM wParam)
{
	switch (MessageIndex)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'w':
		case 'W':
			m_Player->MoveForward();
			break;

		case 's':
		case 'S':
			m_Player->MoveBackward();
			break;

		case 'a':
		case 'A':
			m_Player->MoveLeft();
			break;

		case 'd':
		case 'D':
			m_Player->MoveRight();
			break;
		}
		break;
	}
}