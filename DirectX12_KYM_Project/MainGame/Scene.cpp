#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_Player != nullptr) delete m_Player;
	for (int i = 0; i < m_GameObjects.size(); ++i) if (m_GameObjects[i] != nullptr) delete m_GameObjects[i];
	if (m_UserInterface != nullptr) delete m_UserInterface;
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_RANGE DescriptorRange;
	// Texture Count 1
	ZeroMemory(&DescriptorRange, sizeof(DescriptorRange));
	DescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange.BaseShaderRegister = 0;
	DescriptorRange.RegisterSpace = 0;
	DescriptorRange.NumDescriptors = 1;
	DescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameter[3];
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
	RootParameter[2].DescriptorTable.pDescriptorRanges = &DescriptorRange;
	RootParameter[2].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc;
	ZeroMemory(&StaticSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
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
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �Է�-����(IA) �ܰ� ���
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = 3;
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
	// ���ҽ��� �׷��Ƚ� ���������ο� �����ϴ� ������ �ϴ� RootSignature ����
	CreateRootSignature(Device);

	// Camera�� ������ ������ �÷��̾ ���� �����ϴ� ������Ʈ�� Player ����
	m_Player = new Player();
	m_Player->CreateGameObject(Device, CommandList, m_RootSignature);

	// ���� ���忡 �����ϴ� Game Object ����
	m_GameObjects.reserve(100);
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			m_GameObjects.emplace_back(new GameObject());
			m_GameObjects.back()->CreateGameObject(Device, CommandList, m_RootSignature);
			m_GameObjects.back()->SetPosition(DirectX::XMFLOAT3(-225.f + (i * 50), -50.f, 50.f + (50 * j)));
		}
	}
	// ���ӿ� �ʿ��� UI ����
	m_UserInterface = new UserInterface();
	m_UserInterface->CreateGameObject(Device, CommandList, m_RootSignature);
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	if (m_Player != nullptr) m_Player->Render(CommandList);
	for (int i = 0; i < m_GameObjects.size(); ++i) if (m_GameObjects[i] != nullptr) m_GameObjects[i]->Render(CommandList);
	if (m_UserInterface != nullptr) m_UserInterface->Render(CommandList);
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