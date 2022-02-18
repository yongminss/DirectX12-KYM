#include "stdafx.h"
#include "Scene.h"

#include "Player.h"
#include "Terrain.h"
#include "Skybox.h"
#include "UserInterface.h"
#include "Billboard.h"
#include "InstancingModel.h"

#define DIRECTIONAL_LIGHT 0
#define SPOT_LIGHT 1
#define POINT_LIGHT 2


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_LightBuffer != nullptr) m_LightBuffer->Release();

	if (m_Player != nullptr) delete m_Player;
	if (m_Terrain != nullptr) delete m_Terrain;
	if (m_Skybox != nullptr) delete m_Skybox;
	if (m_HpBar != nullptr) delete m_HpBar;
	if (m_HpGauge != nullptr) delete m_HpGauge;

	if (m_Grass != nullptr) delete m_Grass;
	if (m_Tree != nullptr) delete m_Tree;
	if (m_WeakOrcs != nullptr) delete m_WeakOrcs;
	if (m_StrongOrcs != nullptr) delete m_StrongOrcs;
	if (m_ShamanOrcs != nullptr) delete m_ShamanOrcs;
	if (m_WolfRiderOrcs != nullptr) delete m_WolfRiderOrcs;
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_RANGE DescriptorRange[3];
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
	// Binary Model Texture
	DescriptorRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[2].BaseShaderRegister = 3;
	DescriptorRange[2].RegisterSpace = 0;
	DescriptorRange[2].NumDescriptors = 1;
	DescriptorRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameter[8];
	ZeroMemory(RootParameter, sizeof(RootParameter));
	// Camera
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[0].Constants.ShaderRegister = 0;
	RootParameter[0].Constants.RegisterSpace = 0;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// GameObject World Pos
	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[1].Constants.ShaderRegister = 1;
	RootParameter[1].Constants.RegisterSpace = 0;
	RootParameter[1].Constants.Num32BitValues = 32;
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
	// Binary Loaded Model
	RootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[4].DescriptorTable.pDescriptorRanges = &DescriptorRange[2];
	RootParameter[4].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Skinned Model - Offset
	RootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[5].Descriptor.ShaderRegister = 2;
	RootParameter[5].Descriptor.RegisterSpace = 0;
	RootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Skinned Model - Transform
	RootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[6].Descriptor.ShaderRegister = 3;
	RootParameter[6].Descriptor.RegisterSpace = 0;
	RootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Lighting
	RootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[7].Descriptor.ShaderRegister = 4;
	RootParameter[7].Descriptor.RegisterSpace = 0;
	RootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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
	StaticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력-조립(IA) 단계 허용
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = _countof(RootParameter);
	RootSignatureDesc.pStaticSamplers = &StaticSamplerDesc;
	RootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob *RootSignatureBlob = nullptr;
	ID3DBlob *ErrorBlob = nullptr;

	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_RootSignature);

	if (RootSignatureBlob != nullptr) RootSignatureBlob->Release();
	if (ErrorBlob != nullptr) ErrorBlob->Release();
}

void Scene::CreateLightShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	m_Light.m_Ambient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.f);
	m_Light.m_Diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	m_Light.m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_Light.m_Emissive = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_Light.m_Direction = DirectX::XMFLOAT3(-0.24f, 0.f, -0.43f);
	m_Light.m_Active = true;
	m_Light.m_Type = DIRECTIONAL_LIGHT;

	UINT BufferSize = ((sizeof(MAPPING_LIGHT) + 255) & ~255);

	m_LightBuffer = CreateBuffer(Device, CommandList, nullptr, BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_LightBuffer->Map(0, nullptr, (void**)&m_MappingLight);
}

void Scene::CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// 리소스를 그래픽스 파이프라인에 연결하는 역할을 하는 RootSignature 생성
	CreateRootSignature(Device);

	// Scene에서 사용할 조명을 설정 & Light Buffer 생성
	CreateLightShaderBuffer(Device, CommandList);

	// Camera를 가지고 있으며 플레이어가 직접 조종하는 오브젝트인 Player 생성
	m_Player = new Player(Device, CommandList, m_RootSignature);
	m_Player->SetScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_Player->SetPosition(DirectX::XMFLOAT3(MAP_SIZE / 2.f, 0.f, MAP_SIZE / 2.f));

	// 각 정점 마다 높낮이가 다른 지형(Terrain) 생성
	m_Terrain = new Terrain(Device, CommandList, m_RootSignature);

	// 게임의 배경 역할을 하는 Skybox 생성
	m_Skybox = new Skybox(Device, CommandList, m_RootSignature);
	m_Skybox->UpdateTransform(nullptr);

	// 게임에 필요한 UI 생성
	m_HpBar = new UserInterface(Device, CommandList, m_RootSignature, T_HPBAR);
	m_HpBar->SetPosition(DirectX::XMFLOAT3(-0.5f, 0.9f, 0.f));

	m_HpGauge = new UserInterface(Device, CommandList, m_RootSignature, T_HPGAUGE);
	m_HpGauge->SetPosition(DirectX::XMFLOAT3(-0.51f, 0.9f, 0.f));

	// Scene에 등장하는 Billboard (ex. Grass, Tree ... etc)를 생성
	m_Grass = new Billboard();
	m_Grass->CreateShader(Device, m_RootSignature);
	m_Grass->CreateBillboard(Device, CommandList, m_RootSignature, m_Terrain, T_GRASS, 10000);

	m_Tree = new Billboard();
	m_Tree->CreateShader(Device, m_RootSignature);
	m_Tree->CreateBillboard(Device, CommandList, m_RootSignature, m_Terrain, T_TREE, 100);

	// 게임 월드에 등장하는 Game Objects 생성
	m_WeakOrcs = new InstancingSkinnedModel();
	m_WeakOrcs->CreateShader(Device, m_RootSignature);
	m_WeakOrcs->CreateModel(Device, CommandList, m_RootSignature, M_WEAKORC, 20);

	m_StrongOrcs = new InstancingSkinnedModel();
	m_StrongOrcs->CreateShader(Device, m_RootSignature);
	m_StrongOrcs->CreateModel(Device, CommandList, m_RootSignature, M_STRONGORC, 20);

	m_ShamanOrcs = new InstancingSkinnedModel();
	m_ShamanOrcs->CreateShader(Device, m_RootSignature);
	m_ShamanOrcs->CreateModel(Device, CommandList, m_RootSignature, M_SHAMANORC, 20);

	m_WolfRiderOrcs = new InstancingSkinnedModel();
	m_WolfRiderOrcs->CreateShader(Device, m_RootSignature);
	m_WolfRiderOrcs->CreateModel(Device, CommandList, m_RootSignature, M_WOLFRIDERORC, 20);
}

void Scene::UpdateLightShaderBuffer(ID3D12GraphicsCommandList* CommandList)
{
	memcpy(&(m_MappingLight->m_Light), &m_Light, sizeof(LIGHT));

	D3D12_GPU_VIRTUAL_ADDRESS LightGpuVirtualAddress = m_LightBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(7, LightGpuVirtualAddress);
}

void Scene::Animate(float ElapsedTime, HWND Hwnd)
{
	if (m_Player != nullptr) {
		int GetHeightMapX = int(m_Player->GetPosition().x) / MAP_SCALE, GetHeightMapZ = int(m_Player->GetPosition().z) / MAP_SCALE;
		float GetHeightMapY = m_Terrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ);
		m_Player->Animate(ElapsedTime, Hwnd, m_PreviousPos, GetHeightMapY);
		m_Player->UpdateTransform(nullptr);
	}
	if (m_WeakOrcs != nullptr) m_WeakOrcs->Animate(ElapsedTime, m_Terrain);
	if (m_StrongOrcs != nullptr) m_StrongOrcs->Animate(ElapsedTime, m_Terrain);
	if (m_ShamanOrcs != nullptr) m_ShamanOrcs->Animate(ElapsedTime, m_Terrain);
	if (m_WolfRiderOrcs != nullptr) m_WolfRiderOrcs->Animate(ElapsedTime, m_Terrain);

	if (m_Skybox != nullptr) m_Skybox->Animate(ElapsedTime, m_Player->GetPosition());
	if (m_HpGauge != nullptr) m_HpGauge->Animate(ElapsedTime);
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	if (m_Player != nullptr) m_Player->UpdateCamera(CommandList);

	UpdateLightShaderBuffer(CommandList);

	if (m_Player != nullptr) m_Player->Render(CommandList);

	if (m_HpBar != nullptr) m_HpBar->Render(CommandList);
	if (m_HpGauge != nullptr) m_HpGauge->Render(CommandList);

	if (m_Grass != nullptr) m_Grass->Render(CommandList);
	if (m_Tree != nullptr) m_Tree->Render(CommandList);

	if (m_WeakOrcs != nullptr) m_WeakOrcs->Render(CommandList);
	if (m_StrongOrcs != nullptr) m_StrongOrcs->Render(CommandList);
	if (m_ShamanOrcs != nullptr) m_ShamanOrcs->Render(CommandList);
	if (m_WolfRiderOrcs != nullptr) m_WolfRiderOrcs->Render(CommandList);

	if (m_Terrain != nullptr) m_Terrain->Render(CommandList);
	if (m_Skybox != nullptr) m_Skybox->Render(CommandList);
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

		case VK_SHIFT:
		{
			m_Player->SetAnimationTrack(P_ROLL, ANIMATION_TYPE_ONCE);
		}
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
	case WM_LBUTTONDOWN:
	{
		SetCapture(Hwnd);
		SetCursor(NULL);
		GetCursorPos(&m_PreviousPos);

		// 플레이어가 다른 행동을 하지 않은 상태 (IDLE) 일때만 공격하도록 설정
		if (m_Player->GetCurrentAnimationTrackIndex() == P_IDLE) {
			m_Player->SetAnimationTrack(P_SHOOT, ANIMATION_TYPE_ONCE);

			// 카메라의 Look, Position 좌표를 이용하여 몬스터 오브젝트와 충돌처리 수행
			DirectX::XMFLOAT3 StartPosition = m_Player->GetCameraWorldPosition();
			DirectX::XMFLOAT3 EndPosition{};
			DirectX::XMStoreFloat3(&EndPosition, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_Player->GetCameraWorldLook())));

			/*GameObject *CollisionMonster = new GameObject();

			for (int i = 0; i < m_Monsters.size(); ++i) {
				CollisionMonster = m_GameObjects[i]->CheckCollision(StartPosition, EndPosition);

				if (CollisionMonster != nullptr) {
					std::cout << i << "번째 오브젝트와 충돌거리 : " << CollisionMonster->GetCollisionMeshDistance() << std::endl;

					m_GameObjects[i]->SetAnimationTrack(P_DAMAGED, ANIMATION_TYPE_ONCE);
				}
			}*/
		}
	}
	break;
	}
}