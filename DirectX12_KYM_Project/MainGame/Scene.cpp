#include "stdafx.h"
#include "Scene.h"

#include "Player.h"
#include "Terrain.h"
#include "MultipleTexture.h"
#include "UserInterface.h"
#include "Billboard.h"
#include "Effect.h"
#include "Monster.h"
#include "InstancingModel.h"
#include "Camera.h"

#include "Material.h"
#include "Texture.h"

#define DIRECTIONAL_LIGHT 0
#define SPOT_LIGHT 1
#define POINT_LIGHT 2


ID3D12DescriptorHeap* Scene::m_CbvSrvDescriptorHeap = nullptr;

D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_CpuDescriptorStartHandle{};
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_GpuDescriptorStartHandle{};

D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_CpuDescriptorNextHandle{};
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_GpuDescriptorNextHandle{};


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_CbvSrvDescriptorHeap != nullptr) m_CbvSrvDescriptorHeap->Release();

	if (m_LightBuffer != nullptr) m_LightBuffer->Release();
	if (m_NoiseBuffer != nullptr) m_NoiseBuffer->Release();
	if (m_DistortionBuffer != nullptr) m_DistortionBuffer->Release();

	if (m_TitleScreen != nullptr) delete m_TitleScreen;
	if (m_Selection != nullptr) delete m_Selection;

	if (m_Player != nullptr) delete m_Player;
	if (m_Terrain != nullptr) delete m_Terrain;
	if (m_Skybox != nullptr) delete m_Skybox;
	if (m_BillboardGrass != nullptr) delete m_BillboardGrass;
	if (m_BillboardTree != nullptr) delete m_BillboardTree;
	for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) delete m_Walls[i];
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) delete m_Tree[i];

	for (int i = 0; i < m_Flames.size(); ++i) if (m_Flames[i] != nullptr) delete m_Flames[i];
	if (m_Smoke != nullptr) delete m_Smoke;
	if (m_Spark != nullptr) delete m_Spark;
	if (m_Signal != nullptr) delete m_Signal;
	if (m_FireBall != nullptr) delete m_FireBall;

	if (m_HpBar != nullptr) delete m_HpBar;
	if (m_HpGauge != nullptr) delete m_HpGauge;
	if (m_Aim != nullptr) delete m_Aim;
	for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) delete m_Numbers[i];
	if (m_GameOverScreen != nullptr) delete m_GameOverScreen;
	if (m_EnterFire != nullptr) delete m_EnterFire;
	if (m_EnterMonster != nullptr) delete m_EnterMonster;
	if (m_GuideArea != nullptr) delete m_GuideArea;
	for (int i = 0; i < 3; ++i) if (m_GuidePos[i] != nullptr) delete m_GuidePos[i];

	for (int i = 0; i < m_WeakOrcs.size(); ++i) if (m_WeakOrcs[i] != nullptr) delete m_WeakOrcs[i];
	for (int i = 0; i < m_StrongOrcs.size(); ++i) if (m_StrongOrcs[i] != nullptr) delete m_StrongOrcs[i];
	for (int i = 0; i < m_ShamanOrcs.size(); ++i) if (m_ShamanOrcs[i] != nullptr) delete m_ShamanOrcs[i];
	for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) if (m_WolfRiderOrcs[i] != nullptr) delete m_WolfRiderOrcs[i];
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_RANGE DescriptorRange[4];
	ZeroMemory(&DescriptorRange, sizeof(DescriptorRange));
	// Texture Count : 1
	DescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[0].BaseShaderRegister = 0;
	DescriptorRange[0].RegisterSpace = 0;
	DescriptorRange[0].NumDescriptors = 1;
	DescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// Texture Count : 3 - Flame Object
	DescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[1].BaseShaderRegister = 1;
	DescriptorRange[1].RegisterSpace = 0;
	DescriptorRange[1].NumDescriptors = 3;
	DescriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// Texture Count : 2 - Terrain Object
	DescriptorRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[2].BaseShaderRegister = 4;
	DescriptorRange[2].RegisterSpace = 0;
	DescriptorRange[2].NumDescriptors = 2;
	DescriptorRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// Texture Count : 1 - Binary Model Texture
	DescriptorRange[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange[3].BaseShaderRegister = 6;
	DescriptorRange[3].RegisterSpace = 0;
	DescriptorRange[3].NumDescriptors = 1;
	DescriptorRange[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameter[11];
	ZeroMemory(RootParameter, sizeof(RootParameter));
	// Camera Buffer
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[0].Constants.ShaderRegister = 0;
	RootParameter[0].Constants.RegisterSpace = 0;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// GameObject Buffer
	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[1].Constants.ShaderRegister = 1;
	RootParameter[1].Constants.RegisterSpace = 0;
	RootParameter[1].Constants.Num32BitValues = 32;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Light Buffer
	RootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[2].Constants.ShaderRegister = 2;
	RootParameter[2].Constants.RegisterSpace = 0;
	RootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Descriptor Range 0 - Texture 1
	RootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[3].DescriptorTable.pDescriptorRanges = &DescriptorRange[0];
	RootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Descriptor Range 1 - Texture 3 (Flame)
	RootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[4].DescriptorTable.pDescriptorRanges = &DescriptorRange[1];
	RootParameter[4].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Noise Buffer
	RootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[5].Constants.ShaderRegister = 3;
	RootParameter[5].Constants.RegisterSpace = 0;
	RootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Distortion Buffer
	RootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[6].Constants.ShaderRegister = 4;
	RootParameter[6].Constants.RegisterSpace = 0;
	RootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Descriptor Range 1 - Texture 2 (Terrain)
	RootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[7].DescriptorTable.pDescriptorRanges = &DescriptorRange[2];
	RootParameter[7].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Descriptor Range 3 - Texture 1 (Binary Model)
	RootParameter[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[8].DescriptorTable.pDescriptorRanges = &DescriptorRange[3];
	RootParameter[8].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Bone Offset Buffer
	RootParameter[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[9].Constants.ShaderRegister = 5;
	RootParameter[9].Constants.RegisterSpace = 0;
	RootParameter[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Bone Transform Buffer
	RootParameter[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[10].Constants.ShaderRegister = 6;
	RootParameter[10].Constants.RegisterSpace = 0;
	RootParameter[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC SamplerDesc[2];
	ZeroMemory(SamplerDesc, sizeof(SamplerDesc));
	// �ؽ�ó�� ������ ������ �� ����ϴ� ���÷�
	SamplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc[0].MipLODBias = 0;
	SamplerDesc[0].MaxAnisotropy = 1;
	SamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	SamplerDesc[0].MinLOD = 0;
	SamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc[0].ShaderRegister = 0;
	SamplerDesc[0].RegisterSpace = 0;
	SamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// Flame Objects�� ������ ���� ���÷�
	SamplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc[1].MipLODBias = 0;
	SamplerDesc[1].MaxAnisotropy = 1;
	SamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	SamplerDesc[1].MinLOD = 0;
	SamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc[1].ShaderRegister = 1;
	SamplerDesc[1].RegisterSpace = 0;
	SamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �Է�-����(IA) �ܰ� ���
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = _countof(RootParameter);
	RootSignatureDesc.pStaticSamplers = SamplerDesc;
	RootSignatureDesc.NumStaticSamplers = _countof(SamplerDesc);

	ID3DBlob *RootSignatureBlob = nullptr;
	ID3DBlob *ErrorBlob = nullptr;

	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_RootSignature);

	if (RootSignatureBlob != nullptr) RootSignatureBlob->Release();
	if (ErrorBlob != nullptr) ErrorBlob->Release();
}

void Scene::CreateCbvSrvDescriptorHeap(ID3D12Device* Device, int ConstantBufferViewCount, int ShaderResourceViewCount)
{
	DescriptorHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �ؽ�ó ���ο� �ʿ��� ShaderResource View�� ����� ���� Descriptor Heap�� ����
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescriptorHeapDesc.NumDescriptors = ConstantBufferViewCount + ShaderResourceViewCount;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescriptorHeapDesc.NodeMask = 0;
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_CbvSrvDescriptorHeap);

	m_CpuDescriptorNextHandle = m_CpuDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_GpuDescriptorNextHandle = m_GpuDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void Scene::CreateShaderResourceView(ID3D12Device* Device, Texture* UsingTexture, int RootParameterIndex)
{
	// ������Ʈ�� ����� �ؽ�ó�� ������ ������
	int TextureCount = UsingTexture->GetTextureCount();

	for (int i = 0; i < TextureCount; ++i) {
		// Texture�� ������ �ִ� Texture Buffer�� ������
		ID3D12Resource *TextureBuffer = UsingTexture->GetTextureBuffer(i);
		D3D12_RESOURCE_DESC ResourceDesc = TextureBuffer->GetDesc();

		// ShaderResource View�� ����
		D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = ResourceDesc.Format;
		ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = -1;
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.f;

		Device->CreateShaderResourceView(TextureBuffer, &ShaderResourceViewDesc, m_CpuDescriptorNextHandle);
		m_CpuDescriptorNextHandle.ptr += DescriptorHandleIncrementSize;

		UsingTexture->SetRootArgument(i, RootParameterIndex, m_GpuDescriptorNextHandle);
		m_GpuDescriptorNextHandle.ptr += DescriptorHandleIncrementSize;
	}
}

void Scene::CreateConstantBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// Scene���� ����ϴ� ���� ���� ����
	m_Light.m_Ambient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.f);
	m_Light.m_Diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	m_Light.m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_Light.m_Emissive = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_Light.m_Direction = DirectX::XMFLOAT3(-0.24f, 0.f, -0.43f);
	m_Light.m_Type = DIRECTIONAL_LIGHT;
	m_Light.m_Active = true;

	UINT BufferSize = sizeof(MAPPING_LIGHT) + 12; // 84 + 12

	m_LightBuffer = CreateBuffer(Device, CommandList, nullptr, BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_LightBuffer->Map(0, nullptr, (void**)&m_MappingLight);

	// �Ҳ� ȿ���� ����ϴ� Noise && Distortion ���� ����
	m_Noise.m_FrameTime = 0.f;
	m_Noise.m_ScrollSpeed = DirectX::XMFLOAT3(1.3f, 2.1f, 2.3f);
	m_Noise.m_Scale = DirectX::XMFLOAT3(1.f, 2.f, 3.f);

	UINT NoiseBufferSize = sizeof(MAPPING_NOISE) + 4; // 28 + 4

	m_NoiseBuffer = CreateBuffer(Device, CommandList, nullptr, NoiseBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_NoiseBuffer->Map(0, nullptr, (void**)&m_MappingNoise);

	m_Distortion.m_Distortion1 = DirectX::XMFLOAT2(0.1f, 0.2f);
	m_Distortion.m_Distortion2 = DirectX::XMFLOAT2(0.1f, 0.3f);
	m_Distortion.m_Distortion3 = DirectX::XMFLOAT2(0.1f, 0.1f);
	m_Distortion.m_Scale = 0.5f;
	m_Distortion.m_Bias = 0.8f;

	UINT DistortionBufferSize = sizeof(MAPPING_DISTORTION); // 32

	m_DistortionBuffer = CreateBuffer(Device, CommandList, nullptr, DistortionBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_DistortionBuffer->Map(0, nullptr, (void**)&m_MappingDistortion);
}

void Scene::CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// ���ҽ��� �׷��Ƚ� ���������ο� �����ϴ� ������ �ϴ� RootSignature ����
	CreateRootSignature(Device);

	// ������Ʈ ���� ConstantBuffer View or ShaderResource View�� ����ϸ� Set�� ȣ���� �ʹ� �������Ƿ� �� ���� ���
	CreateCbvSrvDescriptorHeap(Device, 0, 1200);

	// Loaded & Skinned Model�� ����� Shader�� Frame ���� �����ϸ� �޸� ����� �������Ƿ� �̸� ������ �ϳ��� Shader�� ���
	Material::PrepareShader(Device, m_RootSignature);

	// Light, Noise, Distortion�� Constant Buffer ����
	CreateConstantBuffer(Device, CommandList);

	// Title State���� ����� ȭ�� (ex. ���� ���� or ���� or ���� ��)
	m_TitleScreen = new UserInterface(Device, CommandList, m_RootSignature, T_TITLESCREEN);
	m_TitleScreen->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// Title State���� �÷��̾ � �ൿ�� ���� ���� ����â
	m_Selection = new UserInterface(Device, CommandList, m_RootSignature, T_SELECTION);
	m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.125f, 0.f));

	// Camera�� ������ ������ �÷��̾ ���� �����ϴ� ������Ʈ�� Player ����
	m_Player = new Player(Device, CommandList, m_RootSignature);
	m_Player->SetScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_Player->SetPosition(DirectX::XMFLOAT3(MAP_SIZE / 2.f, 0.f, (MAP_SIZE / 2.f) - 300.f));

	// �� ���� ���� �����̰� �ٸ� ����(Terrain) ����
	m_Terrain = new Terrain(Device, CommandList, m_RootSignature);

	// ������ ��� ������ �ϴ� Skybox ����
	m_Skybox = new MultipleTexture(Device, CommandList, m_RootSignature, T_SKYBOX);

	// Scene�� �����ϴ� Billboard (ex. Grass, Tree ... etc)�� ����
	m_BillboardGrass = new Billboard();
	m_BillboardGrass->CreateShader(Device, m_RootSignature);
	m_BillboardGrass->CreateBillboard(Device, CommandList, m_RootSignature, m_Terrain, T_BILLBOARDGRASS, 400);

	m_BillboardTree = new Billboard();
	m_BillboardTree->CreateShader(Device, m_RootSignature);
	m_BillboardTree->CreateBillboard(Device, CommandList, m_RootSignature, m_Terrain, T_BILLBOARDTREE, 625);

	// Billboard Tree -> Tree ��ȯ�� ���� Billboard Tree�� Index�� ����
	int BillTreeIndex = 0;
	for (int z = 0; z < 25; ++z) for (int x = 0; x < 25; ++x) m_SaveBillboardTreeIndex[x][z] = BillTreeIndex++;

	// Player�� Map ������ �̵��ϴ� ���� �����ϴ� Wall ����
	m_Walls = new Billboard*[4];
	for (int i = 0; i < 4; ++i) {
		m_Walls[i] = new Billboard();
		m_Walls[i]->CreateShader(Device, m_RootSignature);
		m_Walls[i]->CreateBillboard(Device, CommandList, m_RootSignature, nullptr, T_WALL + i, 75);
	}

	// Player���� ����� Billboard Tree�� ���� ���� Mesh�� ���� Tree�� �ٲٱ� ���� ����
	int TreeCount = 9;
	m_Tree.reserve(TreeCount);
	for (int i = 0; i < TreeCount; ++i) m_Tree.emplace_back(new MultipleTexture(Device, CommandList, m_RootSignature, T_TREE));

	// Scene�� �����ϴ� Effect ����
	int FlameCount = 1;
	m_Flames.reserve(FlameCount);
	DirectX::XMFLOAT3 FlamePos{};

	// FireArea Pos = x (0 - 2500), z (2500 - 5000)
	for (int i = 0; i < FlameCount; ++i) {
		m_Flames.emplace_back(new Effect(Device, CommandList, m_RootSignature, T_FLAME));
		m_Flames.back()->SetPosition(DirectX::XMFLOAT3(1500.f, 200.f, 3000.f));
	}

	m_Smoke = new Effect(Device, CommandList, m_RootSignature, T_SMOKE);
	m_Smoke->SetPosition(DirectX::XMFLOAT3(m_Player->GetPosition().x, 250.f, m_Player->GetPosition().z));

	m_Spark = new Effect(Device, CommandList, m_RootSignature, T_SPARK);

	m_Signal = new Effect(Device, CommandList, m_RootSignature, T_SIGNAL);

	m_FireBall = new MultipleTexture(Device, CommandList, m_RootSignature, T_FIREBALL);

	//Scene�� �����ϴ� UI ����
	m_HpBar = new UserInterface(Device, CommandList, m_RootSignature, T_HPBAR);
	m_HpBar->SetPosition(DirectX::XMFLOAT3(-0.5f, 0.9f, 0.f));

	m_HpGauge = new UserInterface(Device, CommandList, m_RootSignature, T_HPGAUGE);
	m_HpGauge->SetPosition(DirectX::XMFLOAT3(0.f, 0.9f, 0.f));

	m_Aim = new UserInterface(Device, CommandList, m_RootSignature, T_AIM);
	m_Aim->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	m_Numbers = new UserInterface*[2];
	m_Numbers[0] = new UserInterface(Device, CommandList, m_RootSignature, T_NUMBERS);
	m_Numbers[0]->SetPosition(DirectX::XMFLOAT3(0.725f, -0.85f, 0.f));

	m_Numbers[1] = new UserInterface(Device, CommandList, m_RootSignature, T_NUMBERS);
	m_Numbers[1]->SetPosition(DirectX::XMFLOAT3(0.85f, -0.85f, 0.f));

	m_GameOverScreen = new UserInterface(Device, CommandList, m_RootSignature, T_GAMEOVER);
	m_GameOverScreen->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	m_EnterFire = new UserInterface(Device, CommandList, m_RootSignature, T_ENTERFIRE);
	m_EnterFire->SetPosition(DirectX::XMFLOAT3(-0.5f, -0.75f, 0.f));

	m_EnterMonster = new UserInterface(Device, CommandList, m_RootSignature, T_ENTERMONSTER);
	m_EnterMonster->SetPosition(DirectX::XMFLOAT3(-0.5f, -0.75f, 0.f));

	m_GuideArea = new UserInterface(Device, CommandList, m_RootSignature, T_GUIDEAREA);
	m_GuideArea->SetPosition(DirectX::XMFLOAT3(0.375f, 0.875f, 0.f));

	m_GuidePos = new UserInterface*[3];
	for (int i = 0; i < 3; ++i) {
		m_GuidePos[i] = new UserInterface(Device, CommandList, m_RootSignature, T_GUIDENONE + i);
		if (i == 2) m_GuidePos[i]->SetPosition(DirectX::XMFLOAT3(0.75f, 0.875f, 0.f));
		else m_GuidePos[i]->SetPosition(DirectX::XMFLOAT3(0.675f, 0.875f, 0.f));
	}

	int WeakOrcCount = 50;
	m_WeakOrcs.reserve(WeakOrcCount);
	DirectX::XMFLOAT3 WeakOrcPos{};

	for (int i = 0; i < WeakOrcCount; ++i) {
		GameObject* Model = nullptr;
		Model = GameObject::LoadBinaryFileModel(Device, CommandList, m_RootSignature, "Model/Monster_WeakOrc.bin", nullptr, true);
		m_WeakOrcs.emplace_back(new Monster(M_WEAKORC));
		m_WeakOrcs.back()->SetChild(Model);
		m_WeakOrcs.back()->SetAnimationTrack(M_IDLE, ANIMATION_TYPE_LOOP);
		WeakOrcPos.x = 200.f + rand() % 2250, WeakOrcPos.z = 200.f + rand() % 2250;
		m_WeakOrcs.back()->SetPosition(WeakOrcPos);
	}

	// MonsterArea Pos = x (2500 - 5000), z (2500 - 5000)
	int StrongOrcCount = 10;
	m_StrongOrcs.reserve(StrongOrcCount);
	DirectX::XMFLOAT3 StrongOrcPos{};

	for (int i = 0; i < StrongOrcCount; ++i) {
		GameObject* Model = nullptr;
		Model = GameObject::LoadBinaryFileModel(Device, CommandList, m_RootSignature, "Model/Monster_StrongOrc.bin", nullptr, true);
		m_StrongOrcs.emplace_back(new Monster(M_STRONGORC));
		m_StrongOrcs.back()->SetChild(Model);
		m_StrongOrcs.back()->SetAnimationTrack(M_IDLE, ANIMATION_TYPE_LOOP);
		if (i < 5) {
			StrongOrcPos.x = 2700.f + (i * 525.f);
			StrongOrcPos.z = 2700.f;
		}
		else {
			StrongOrcPos.x = 2700.f;
			StrongOrcPos.z = 2700.f + ((i - 4) * 525.f);
		}
		m_StrongOrcs.back()->SetPosition(StrongOrcPos);
	}

	int ShamanOrcCount = 5;
	m_ShamanOrcs.reserve(ShamanOrcCount);
	DirectX::XMFLOAT3 ShamanOrcPos{};

	for (int i = 0; i < ShamanOrcCount; ++i) {
		GameObject* Model = nullptr;
		Model = GameObject::LoadBinaryFileModel(Device, CommandList, m_RootSignature, "Model/Monster_ShamanOrc.bin", nullptr, true);
		m_ShamanOrcs.emplace_back(new Monster(M_SHAMANORC));
		m_ShamanOrcs.back()->SetChild(Model);
		m_ShamanOrcs.back()->SetAnimationTrack(M_IDLE, ANIMATION_TYPE_LOOP);
		ShamanOrcPos.x = 3000.f + rand() % 1500, ShamanOrcPos.z = 3000.f + rand() % 1500;
		m_ShamanOrcs.back()->SetPosition(ShamanOrcPos);
	}

	int WolfRiderOrcCount = 1;
	m_WolfRiderOrcs.reserve(WolfRiderOrcCount);

	for (int i = 0; i < WolfRiderOrcCount; ++i) {
		GameObject* Model = nullptr;
		Model = GameObject::LoadBinaryFileModel(Device, CommandList, m_RootSignature, "Model/Monster_WolfRiderOrc.bin", nullptr, true);
		m_WolfRiderOrcs.emplace_back(new Monster(M_WOLFRIDERORC));
		m_WolfRiderOrcs.back()->SetChild(Model);
		m_WolfRiderOrcs.back()->SetAnimationTrack(M_IDLE, ANIMATION_TYPE_LOOP);
		m_WolfRiderOrcs.back()->SetPosition(DirectX::XMFLOAT3(4700.f, 0.f, 4700.f));
	}
}

void Scene::UpdateConstantBuffer(ID3D12GraphicsCommandList* CommandList)
{
	memcpy(&(m_MappingLight->m_Light), &m_Light, sizeof(LIGHT));
	D3D12_GPU_VIRTUAL_ADDRESS LightGpuVirtualAddress = m_LightBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(2, LightGpuVirtualAddress);

	memcpy(&(m_MappingNoise->m_Noise), &m_Noise, sizeof(NOISE));
	D3D12_GPU_VIRTUAL_ADDRESS NoiseGpuVirtualAddress = m_NoiseBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(5, NoiseGpuVirtualAddress);

	memcpy(&(m_MappingDistortion->m_Distortion), &m_Distortion, sizeof(DISTORTION));
	D3D12_GPU_VIRTUAL_ADDRESS DistortionGpuVirtualAddress = m_DistortionBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(6, DistortionGpuVirtualAddress);
}

void Scene::UpdateFireArea(float ElapsedTime)
{
	// 1. ������ ��ܿ� ������ ������ ǥ��
	m_ActiveGuidePosIndex = 1;

	// 2. � ������ �����ߴ��� �ؽ�Ʈ�� ���� �˷���
	if (m_EnterMonster != nullptr) m_EnterMonster->Animate(0.f, 0);
	if (m_EnterFire != nullptr) m_EnterFire->Animate(ElapsedTime, 1);

	// 3. ���� �ñ⿡ ��, ������ ���� ������ ����
	if (0.25f >= m_ChangeColorTime) m_ChangeColorTime += ElapsedTime * 0.125f;

	// 3-1. Map Object
	if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	if (m_BillboardGrass != nullptr) m_BillboardGrass->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) m_Walls[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
	// 3-2. Model Object
	if (m_Player != nullptr) m_Player->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f));
}

void Scene::UpdateMonsterArea(float ElapsedTime)
{
	// 1. ������ ��ܿ� ������ ������ ǥ��
	m_ActiveGuidePosIndex = 2;

	// 2. � ������ �����ߴ��� �ؽ�Ʈ�� ���� �˷���
	if (m_EnterFire != nullptr) m_EnterFire->Animate(0.f, 0);
	if (m_EnterMonster != nullptr) m_EnterMonster->Animate(ElapsedTime, 1);

	// 3. ���� �ñ⿡ ��, ������ ���� ������ ����
	if (0.25f >= m_ChangeColorTime) m_ChangeColorTime += ElapsedTime * 0.125f;

	// 3-1. Map Object
	if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	if (m_BillboardGrass != nullptr) m_BillboardGrass->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) m_Walls[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	// 3-2. Model Object
	if (m_Player != nullptr) m_Player->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	for (int i = 0; i < m_StrongOrcs.size(); ++i) if (m_StrongOrcs[i] != nullptr) m_StrongOrcs[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	for (int i = 0; i < m_ShamanOrcs.size(); ++i) if (m_ShamanOrcs[i] != nullptr) m_ShamanOrcs[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
	for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) if (m_WolfRiderOrcs[i] != nullptr) m_WolfRiderOrcs[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime));
}

void Scene::Animate(float ElapsedTime, HWND Hwnd)
{
	switch (m_State) {
	case STATE_TITLE:
	{

	}
	break;

	case STATE_MAIN:
	{
		if (m_Player != nullptr) {
			// 1. Terrain�� ��ġ�� ���� Player�� ���� ��ǥ y���� ����
			float PlayerxPos = m_Player->GetPosition().x, PlayerzPos = m_Player->GetPosition().z;

			int GetHeightMapX = int(PlayerxPos) / MAP_SCALE, GetHeightMapZ = int(PlayerzPos) / MAP_SCALE;
			float GetHeightMapY = m_Terrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ);

			// 2. Player�� �ִϸ��̼� ����
			m_Player->Animate(ElapsedTime, Hwnd, m_PreviousPos, GetHeightMapY);
			m_Player->UpdateTransform(nullptr);

			// 2-1. ���� �ִϸ��̼� ������ �Ϸ� �ÿ� �Ѿ� ���� �ִ� ����
			if (true == m_Player->GetCompletedReload()) {
				m_BulletCount = 30;
				m_Player->SetCompletedReload(false);
			}
		}

		// �÷��̾��� ���� ��ġ�� ���� Area (None, Fire, Monster) ����
		float PlayerxPos = m_Player->GetPosition().x, PlayerzPos = m_Player->GetPosition().z;
		// 1. Fire Area - ( x [0 - 2500], z [2500 - 5000] )
		if ((0 <= PlayerxPos && 2500 > PlayerxPos) && (2500 <= PlayerzPos/* && 5000 > PlayerzPos*/)) {
			UpdateFireArea(ElapsedTime);
		}
		// 2. Monster Area - ( x [2500 - 5000], z [2500 - 5000] )
		else if ((2500 <= PlayerxPos /*&& 5000 > PlayerxPos*/) && (2500 <= PlayerzPos /*&& 5000 > PlayerzPos*/)) {
			UpdateMonsterArea(ElapsedTime);
		}
		// 3. None Area - ������ ����
		else {
			m_ActiveGuidePosIndex = 0;

			if (m_EnterFire != nullptr) m_EnterFire->Animate(0.f, 0);
			if (m_EnterMonster != nullptr) m_EnterMonster->Animate(0.f, 0);

			if (0.f <= m_ChangeColorTime) m_ChangeColorTime -= ElapsedTime * 0.125f;

			// �ʰ� �� ������Ʈ�� ������ ������� ����
			if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			if (m_BillboardGrass != nullptr) m_BillboardGrass->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) m_Walls[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
			if (m_Player != nullptr) m_Player->SetChangeTexcoords(DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime));
		}

		// �÷��̾�� ���� ���� �浹 ó�� �ִϸ��̼� ���� (ex. ������ �÷��̾� �ν�, ���� ��)
		bool PlayerHit = false;

		for (int i = 0; i < m_WeakOrcs.size(); ++i)
			if (m_WeakOrcs[i] != nullptr) {
				m_WeakOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_WeakOrcs[i]->UpdateTransform(nullptr);

				if (true == m_WeakOrcs[i]->GetSuccessAttack()) PlayerHit = true, m_WeakOrcs[i]->SetSuccessAttack(false);
			}

		for (int i = 0; i < m_StrongOrcs.size(); ++i)
			if (m_StrongOrcs[i] != nullptr) {
				m_StrongOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_StrongOrcs[i]->UpdateTransform(nullptr);

				if (true == m_StrongOrcs[i]->GetSuccessAttack()) PlayerHit = true, m_StrongOrcs[i]->SetSuccessAttack(false);
			}

		for (int i = 0; i < m_ShamanOrcs.size(); ++i)
			if (m_ShamanOrcs[i] != nullptr) {
				m_ShamanOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_ShamanOrcs[i]->UpdateTransform(nullptr);

				if (true == m_ShamanOrcs[i]->GetSuccessAttack()) PlayerHit = true, m_ShamanOrcs[i]->SetSuccessAttack(false);
			}

		for (int i = 0; i < m_WolfRiderOrcs.size(); ++i)
			if (m_WolfRiderOrcs[i] != nullptr) {
				m_WolfRiderOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_WolfRiderOrcs[i]->UpdateTransform(nullptr);

				if (true == m_WolfRiderOrcs[i]->GetSuccessAttack()) PlayerHit = true, m_WolfRiderOrcs[i]->SetSuccessAttack(false);
			}

		// ������ ������ �����ϸ� �÷��̾��� ���� ����
		if (true == PlayerHit) m_Player->ActiveDamaged();

		// �÷��̾�� ����� Billboard Tree�� ���� ���� �ؽ�ó�� ���� Tree�� ����
		int x = int(PlayerxPos / 200), z = int(PlayerzPos / 200);
		DirectX::XMFLOAT4X4 *GetBillboardTreePos = m_BillboardTree->GetObjectsWorldPos();

		for (int RangeX = -1, i = 0; RangeX < 2; ++RangeX) {
			for (int RangeZ = -1; RangeZ < 2; ++RangeZ, ++i) {
				// �÷��̾�� ����� Billboard Tree�� ��ǥ�� Tree�� ����
				int xIndex = x + RangeX, zIndex = z + RangeZ;
				// ���� ���� Index�� ������ �� �����Ƿ� ����ó�� ����
				if (xIndex < 0) xIndex = 0;
				if (xIndex > 24) xIndex = 24;
				if (zIndex < 0) zIndex = 0;
				if (zIndex > 24) zIndex = 24;

				if (m_Tree[i] != nullptr) m_Tree[i]->SetPosition(DirectX::XMFLOAT3(GetBillboardTreePos[m_SaveBillboardTreeIndex[xIndex][zIndex]]._41,
					GetBillboardTreePos[m_SaveBillboardTreeIndex[xIndex][zIndex]]._42, GetBillboardTreePos[m_SaveBillboardTreeIndex[xIndex][zIndex]]._43));
			}
		}

		// Fire Area�� ������ �̿ϼ��̹Ƿ� �ӽ� ����
		m_Noise.m_FrameTime += ElapsedTime * 0.5f;
		if (m_Noise.m_FrameTime > 1000.f) m_Noise.m_FrameTime = 0.f;
		for (int i = 0; i < m_Flames.size(); ++i) m_Flames[i]->Animate(ElapsedTime);

		if (m_Smoke != nullptr) m_Smoke->Animate(ElapsedTime);
		if (m_Spark != nullptr) m_Spark->Animate(ElapsedTime);
		if (m_Signal != nullptr) m_Signal->Animate(ElapsedTime);

		if (m_HpGauge != nullptr) m_HpGauge->Animate(ElapsedTime, m_Player->GetHp());
		for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) m_Numbers[i]->Animate(i, m_BulletCount);

		// �÷��̾��� ��� �� ���� ���� ȭ�� ���
		if (m_Player->GetCurrentAnimationTrackIndex() == P_DEATH) {
			if (m_GameOverScreen != nullptr) {
				m_GameOverScreen->SetActive(true);
				m_GameOverScreen->Animate(ElapsedTime, 0);
			}
		}

		if (m_Skybox != nullptr) m_Skybox->Animate(ElapsedTime, m_Player->GetPosition());
	}
	break;
	}
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	switch (m_State) {
	case STATE_TITLE:
	{
		CommandList->SetGraphicsRootSignature(m_RootSignature);

		CommandList->SetDescriptorHeaps(1, &m_CbvSrvDescriptorHeap);

		D3D12_VIEWPORT Viewport = { 0.f, 0.f, Window_Width, Window_Height, 0.f, 1.f };
		D3D12_RECT ScissorRect = { 0, 0, Window_Width, Window_Height };

		CommandList->RSSetViewports(1, &Viewport);
		CommandList->RSSetScissorRects(1, &ScissorRect);

		if (m_Selection != nullptr) m_Selection->Render(CommandList);
		if (m_TitleScreen != nullptr) m_TitleScreen->Render(CommandList);
	}
	break;

	case STATE_MAIN:
	{
		CommandList->SetGraphicsRootSignature(m_RootSignature);

		CommandList->SetDescriptorHeaps(1, &m_CbvSrvDescriptorHeap);

		if (m_Player != nullptr) m_Player->UpdateCamera(CommandList);

		UpdateConstantBuffer(CommandList);

		// 1. Rendering Map
		if (m_Terrain != nullptr) m_Terrain->Render(CommandList);
		for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) m_Walls[i]->Render(CommandList);

		// 2. Rendering Effect
		for (int i = 0; i < m_Flames.size(); ++i) m_Flames[i]->Render(CommandList);
		if (m_Spark != nullptr) m_Spark->Render(CommandList);
		if (m_Signal != nullptr) m_Signal->Render(CommandList);
		if (m_FireBall != nullptr) m_FireBall->Render(CommandList);

		// 3. Rendering Objects
		if (m_BillboardGrass != nullptr) m_BillboardGrass->Render(CommandList);
		if (m_BillboardTree != nullptr) m_BillboardTree->Render(CommandList);
		for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->Render(CommandList);

		for (int i = 0; i < m_WeakOrcs.size(); ++i) if (m_WeakOrcs[i] != nullptr) m_WeakOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_StrongOrcs.size(); ++i) if (m_StrongOrcs[i] != nullptr) m_StrongOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_ShamanOrcs.size(); ++i) if (m_ShamanOrcs[i] != nullptr) m_ShamanOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) if (m_WolfRiderOrcs[i] != nullptr) m_WolfRiderOrcs[i]->Render(CommandList);

		if (m_Player != nullptr) m_Player->Render(CommandList);

		// 4. Rendering UserInterface
		if (m_GameOverScreen != nullptr) m_GameOverScreen->Render(CommandList);
		if (m_HpBar != nullptr) m_HpBar->Render(CommandList);
		if (m_HpGauge != nullptr) m_HpGauge->Render(CommandList);
		if (m_Aim != nullptr) m_Aim->Render(CommandList);
		if (m_EnterFire != nullptr) m_EnterFire->Render(CommandList);
		if (m_EnterMonster != nullptr) m_EnterMonster->Render(CommandList);
		if (m_GuideArea != nullptr) m_GuideArea->Render(CommandList);
		if (m_GuidePos[m_ActiveGuidePosIndex] != nullptr) m_GuidePos[m_ActiveGuidePosIndex]->Render(CommandList);
		for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) m_Numbers[i]->Render(CommandList);

		// 5. Rendering Blending Objects
		if (m_Smoke != nullptr) m_Smoke->Render(CommandList);

		// 6. Rendering Depth Mask-Zero Objects
		if (m_Skybox != nullptr) m_Skybox->Render(CommandList);
	}
	break;
	}
}

void Scene::KeyboardMessage(UINT MessageIndex, WPARAM Wparam)
{
	switch (m_State) {
	case STATE_TITLE:
	{
		switch (MessageIndex)
		{
		case WM_KEYDOWN:
			switch (Wparam)
			{
			case VK_UP:
			{
				m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.125f, 0.f));
			}
			break;

			case VK_DOWN:
			{
				m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.45f, 0.f));
			}
			break;

			case VK_RETURN:
			{
				if (m_Selection->GetPosition().y == -0.125f) m_State = STATE_MAIN;
				else if (m_Selection->GetPosition().y == -0.45f) PostQuitMessage(0);
			}
			break;
			}
			break;
		}
	}
	break;

	case STATE_MAIN:
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

			case 'r':
			case 'R':
			{
				if (m_BulletCount != 30) m_Player->ActiveReload();
			}
			break;

			case VK_SHIFT:
			{
				m_Player->ActiveRoll();
			}
			break;

			// �׽�Ʈ�� ���� Ű �Է�
			case '1':
			{
				float x, z = 0;
				std::cin >> x;
				std::cin >> z;

				m_Player->SetPosition(DirectX::XMFLOAT3(x, 0.f, z));
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
	break;
	}
}

void Scene::MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam)
{
	switch (m_State) {
	case STATE_TITLE:
	{

	}
	break;

	case STATE_MAIN:
	{
		switch (MessageIndex)
		{
		case WM_LBUTTONDOWN:
		{
			SetCapture(Hwnd);
			SetCursor(NULL);
			GetCursorPos(&m_PreviousPos);

			// �÷��̾� ���� ó�� (ex. �� �߻� �ִϸ��̼� or �� �Ҳ� Ȱ��ȭ or ���� �ǰ� ���� ��)
			if (m_Player->GetCurrentAnimationTrackIndex() != P_ROLL && m_BulletCount > 0 && m_Player->GetCurrentAnimationTrackIndex() < P_SHOOT) {
				--m_BulletCount;
				m_Player->ActiveShoot();

				// �ѱ� �Ҳ� Ȱ��ȭ
				GameObject* PlayerWeapon = m_Player->GetFrame(25);

				// Weapon�� �� ��ǥ�� ����
				DirectX::XMFLOAT4X4 WeaponPos = PlayerWeapon->GetTransformPos();

				// �÷��̾��� ���� ��ġ ������ ���� Weapon�� ���� ��ǥ�� ����
				DirectX::XMFLOAT4X4 WeaponWorldPos{};
				DirectX::XMStoreFloat4x4(&WeaponWorldPos, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&WeaponPos), DirectX::XMLoadFloat4x4(&m_Player->GetWorldPos())));

				// Weapon�� ���� ��ǥ�� �Ѱ���
				if (m_Spark != nullptr) m_Spark->ActiveEffect(WeaponWorldPos);

				// �÷��̾��� Look, Position ��ǥ�� �̿��Ͽ� ���� ������Ʈ�� �浹ó�� ����
				Camera* GetCamera = m_Player->GetCamera();
				DirectX::XMFLOAT3 StartPosition = m_Player->GetPosition();
				DirectX::XMFLOAT3 EndPosition{};
				DirectX::XMStoreFloat3(&EndPosition, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&GetCamera->GetLook())));

				int MonsterType = -1;
				int CollisionMonsterIndex = 0;

				GameObject *CollisionMonster = new GameObject();
				GameObject *ResultMonster = new GameObject();
				float MaxDistance = 800.f;

				for (int i = 0; i < m_WeakOrcs.size(); ++i) {
					CollisionMonster = m_WeakOrcs[i]->CheckCollision(StartPosition, EndPosition);
					if (CollisionMonster != nullptr) {
						// ������ �浹 �Ÿ��� �ִ� ��Ÿ����� ������ �浹 ó��
						if (CollisionMonster->GetCollisionMeshDistance() <= MaxDistance) {
							// �浹�� ���� ��, ���� ����� �Ÿ��� ���͸� �浹 ó���� ����
							ResultMonster = CollisionMonster;
							MaxDistance = CollisionMonster->GetCollisionMeshDistance();
							CollisionMonsterIndex = i;
							MonsterType = M_WEAKORC;
						}
					}
				}

				for (int i = 0; i < m_StrongOrcs.size(); ++i) {
					CollisionMonster = m_StrongOrcs[i]->CheckCollision(StartPosition, EndPosition);
					if (CollisionMonster != nullptr) {
						if (CollisionMonster->GetCollisionMeshDistance() <= MaxDistance) {
							ResultMonster = CollisionMonster;
							MaxDistance = CollisionMonster->GetCollisionMeshDistance();
							CollisionMonsterIndex = i;
							MonsterType = M_STRONGORC;
						}
					}
				}

				for (int i = 0; i < m_ShamanOrcs.size(); ++i) {
					CollisionMonster = m_ShamanOrcs[i]->CheckCollision(StartPosition, EndPosition);
					if (CollisionMonster != nullptr) {
						if (CollisionMonster->GetCollisionMeshDistance() <= MaxDistance) {
							ResultMonster = CollisionMonster;
							MaxDistance = CollisionMonster->GetCollisionMeshDistance();
							CollisionMonsterIndex = i;
							MonsterType = M_SHAMANORC;
						}
					}
				}

				for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) {
					CollisionMonster = m_WolfRiderOrcs[i]->CheckCollision(StartPosition, EndPosition);
					if (CollisionMonster != nullptr) {
						if (CollisionMonster->GetCollisionMeshDistance() <= MaxDistance) {
							ResultMonster = CollisionMonster;
							MaxDistance = CollisionMonster->GetCollisionMeshDistance();
							CollisionMonsterIndex = i;
							MonsterType = M_WOLFRIDERORC;
						}
					}
				}


				if (nullptr != ResultMonster) {
					switch (MonsterType) {
					case M_WEAKORC:
					{
						m_WeakOrcs[CollisionMonsterIndex]->ActiveDamaged();
					}
					break;

					case M_STRONGORC:
					{
						m_StrongOrcs[CollisionMonsterIndex]->ActiveDamaged();
					}
					break;


					case M_SHAMANORC:
					{
						m_ShamanOrcs[CollisionMonsterIndex]->ActiveDamaged();
					}
					break;

					case M_WOLFRIDERORC:
					{
						m_WolfRiderOrcs[CollisionMonsterIndex]->ActiveDamaged();
					}
					break;
					}
				}
			}
		}
		break;
		}
	}
	break;
	}
}