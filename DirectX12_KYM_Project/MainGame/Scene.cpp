#include "stdafx.h"
#include "Scene.h"

#include "Player.h"
#include "Terrain.h"
#include "MultipleTexture.h"
#include "UserInterface.h"
#include "Billboard.h"
#include "Effect.h"
#include "Monster.h"
#include "Camera.h"

#include "Material.h"
#include "Texture.h"


#define AREA_NONE 0
#define AREA_FIRE 1
#define AREA_MONSTER 2


ID3D12DescriptorHeap* Scene::m_CbvSrvDescriptorHeap = nullptr;

D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_CbvCpuDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_CbvGpuDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_SrvCpuDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_SrvGpuDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_CbvCpuDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_CbvGpuDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE Scene::m_SrvCpuDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE Scene::m_SrvGpuDescriptorNextHandle;


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_CbvSrvDescriptorHeap != nullptr) m_CbvSrvDescriptorHeap->Release();

	if (m_LightBuffer != nullptr) { m_LightBuffer->Unmap(0, nullptr), m_LightBuffer->Release(); }
	if (m_NoiseBuffer != nullptr) { m_NoiseBuffer->Unmap(0, nullptr), m_NoiseBuffer->Release(); }
	if (m_DistortionBuffer != nullptr) { m_DistortionBuffer->Unmap(0, nullptr), m_DistortionBuffer->Release(); }

	if (m_TitleScreen != nullptr) delete m_TitleScreen;
	if (m_Selection != nullptr) delete m_Selection;
	if (m_GameManual != nullptr) delete m_GameManual;

	if (m_Player != nullptr) delete m_Player;

	if (m_Terrain != nullptr) delete m_Terrain;
	if (m_Skybox != nullptr) delete m_Skybox;
	if (m_BillboardTree != nullptr) delete m_BillboardTree;
	for (int i = 0; i < 4; ++i) if (m_Walls[i] != nullptr) delete m_Walls[i];
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) delete m_Tree[i];
	if (m_ItemHp != nullptr) delete m_ItemHp;

	for (int i = 0; i < m_Flames.size(); ++i) if (m_Flames[i] != nullptr) delete m_Flames[i];
	for (int i = 0; i < m_Smokes.size(); ++i) if (m_Smokes[i] != nullptr) delete m_Smokes[i];
	for (int i = 0; i < m_Powders.size(); ++i) if (m_Powders[i] != nullptr) delete m_Powders[i];
	if (m_Spark != nullptr) delete m_Spark;
	if (m_Signal != nullptr) delete m_Signal;
	if (m_Headshot != nullptr) delete m_Headshot;

	if (m_HpBar != nullptr) delete m_HpBar;
	if (m_HpGauge != nullptr) delete m_HpGauge;
	if (m_Aim != nullptr) delete m_Aim;
	for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) delete m_Numbers[i];
	if (m_GameOverScreen != nullptr) delete m_GameOverScreen;
	if (m_GameEndScreen != nullptr) delete m_GameEndScreen;
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
	ZeroMemory(DescriptorRange, sizeof(DescriptorRange));
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
	// 텍스처의 색상을 결정할 때 사용하는 샘플러
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
	// Flame Objects의 색상을 섞는 샘플러
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
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력-조립(IA) 단계 허용
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

	// 텍스처 매핑에 필요한 ShaderResource View를 만들기 위해 Descriptor Heap을 생성
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescriptorHeapDesc.NumDescriptors = ConstantBufferViewCount + ShaderResourceViewCount;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescriptorHeapDesc.NodeMask = 0;
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_CbvSrvDescriptorHeap);

	m_CbvCpuDescriptorNextHandle = m_CbvCpuDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_CbvGpuDescriptorNextHandle = m_CbvGpuDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	m_SrvCpuDescriptorNextHandle.ptr = m_SrvCpuDescriptorStartHandle.ptr = m_CbvCpuDescriptorStartHandle.ptr + (DescriptorHandleIncrementSize * ConstantBufferViewCount);
	m_SrvGpuDescriptorNextHandle.ptr = m_SrvGpuDescriptorStartHandle.ptr = m_CbvGpuDescriptorStartHandle.ptr + (DescriptorHandleIncrementSize * ConstantBufferViewCount);
}

void Scene::CreateShaderResourceView(ID3D12Device* Device, Texture* UsingTexture, int RootParameterIndex)
{
	// 오브젝트가 사용할 텍스처의 정보를 가져옴
	int TextureCount = UsingTexture->GetTextureCount();

	for (int i = 0; i < TextureCount; ++i) {
		// Texture가 가지고 있는 Texture Buffer를 가져옴
		ID3D12Resource *TextureBuffer = UsingTexture->GetTextureBuffer(i);
		D3D12_RESOURCE_DESC ResourceDesc = TextureBuffer->GetDesc();

		// ShaderResource View를 생성
		D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = ResourceDesc.Format;
		ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = -1;
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.f;

		Device->CreateShaderResourceView(TextureBuffer, &ShaderResourceViewDesc, m_SrvCpuDescriptorNextHandle);
		m_SrvCpuDescriptorNextHandle.ptr += DescriptorHandleIncrementSize;

		UsingTexture->SetRootArgument(i, RootParameterIndex, m_SrvGpuDescriptorNextHandle);
		m_SrvGpuDescriptorNextHandle.ptr += DescriptorHandleIncrementSize;
	}
}

void Scene::CreateConstantBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// HLSL에서 사용하는 MAPPING_LIGHT의 값을 설정
	m_LightCount = 3;
	m_Lights = new LIGHT[m_LightCount];
	ZeroMemory(m_Lights, sizeof(LIGHT) * m_LightCount);

	// 게임에서 태양 빛으로 사용되는 직접 조명을 설정
	m_Lights[0].m_Ambient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.f);
	m_Lights[0].m_Diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	m_Lights[0].m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Lights[0].m_Emissive = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Lights[0].m_Direction = DirectX::XMFLOAT3(-0.24f, 0.f, -0.43f);
	m_Lights[0].m_Type = DIRECTIONAL_LIGHT;
	m_Lights[0].m_Active = true;

	// 불꽃 효과에 사용하는 점 조명을 설정
	m_Lights[1].m_Ambient = DirectX::XMFLOAT4(1.5f, 0.5f, 0.5f, 1.f);
	m_Lights[1].m_Diffuse = DirectX::XMFLOAT4(0.5f, 0.f, 0.f, 1.f);
	m_Lights[1].m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Lights[1].m_Position = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	m_Lights[1].m_Range = 250.f;
	m_Lights[1].m_Type = POINT_LIGHT;
	m_Lights[1].m_Active = true;
	m_Lights[1].m_Attenuation = DirectX::XMFLOAT3(1.f, 0.001f, 0.0001f);

	// 플레이어의 원형 그림자 표현에 사용되는 점 조명을 설정
	m_Lights[2].m_Ambient = DirectX::XMFLOAT4(-0.5f, -0.5f, -0.5f, 1.f);
	m_Lights[2].m_Diffuse = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Lights[2].m_Specular = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_Lights[2].m_Position = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	m_Lights[2].m_Range = 10.f;
	m_Lights[2].m_Type = POINT_LIGHT;
	m_Lights[2].m_Active = true;
	m_Lights[2].m_Attenuation = DirectX::XMFLOAT3(1.f, 0.01f, 0.001f);

	UINT LightBufferSize = ((sizeof(MAPPING_LIGHT) + 255) & ~255);

	m_LightBuffer = CreateBuffer(Device, CommandList, nullptr, LightBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_LightBuffer->Map(0, nullptr, (void**)&m_MappingLight);

	m_Noise.m_FrameTime = 0.f;
	m_Noise.m_ScrollSpeed = DirectX::XMFLOAT3(1.3f, 2.1f, 2.3f);
	m_Noise.m_Scale = DirectX::XMFLOAT3(1.f, 2.f, 3.f);

	// 불꽃 효과에 사용하는 Noise && Distortion 값을 설정
	UINT NoiseBufferSize = ((sizeof(MAPPING_NOISE) + 255) & ~255);

	m_NoiseBuffer = CreateBuffer(Device, CommandList, nullptr, NoiseBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_NoiseBuffer->Map(0, nullptr, (void**)&m_MappingNoise);

	m_Distortion.m_Distortion1 = DirectX::XMFLOAT2(0.1f, 0.2f);
	m_Distortion.m_Distortion2 = DirectX::XMFLOAT2(0.1f, 0.3f);
	m_Distortion.m_Distortion3 = DirectX::XMFLOAT2(0.1f, 0.1f);
	m_Distortion.m_Scale = 0.5f;
	m_Distortion.m_Bias = 0.8f;

	UINT DistortionBufferSize = ((sizeof(MAPPING_DISTORTION) + 255) & ~255);

	m_DistortionBuffer = CreateBuffer(Device, CommandList, nullptr, DistortionBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_DistortionBuffer->Map(0, nullptr, (void**)&m_MappingDistortion);
}

void Scene::CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// 리소스를 그래픽스 파이프라인에 연결하는 역할을 하는 RootSignature 생성
	CreateRootSignature(Device);

	// 오브젝트 별로 ConstantBuffer View or ShaderResource View를 사용하면 Set의 호출이 너무 많아지므로 한 번에 사용
	CreateCbvSrvDescriptorHeap(Device, 6, 219);

	// Loaded & Skinned Model이 사용할 Shader를 Frame 별로 생성하면 메모리 사용이 많아지므로 미리 생성한 하나의 Shader를 사용
	Material::PrepareShader(Device, m_RootSignature);

	// Light, Noise, Distortion의 Constant Buffer 생성
	CreateConstantBuffer(Device, CommandList);

	// Title State에서 사용할 화면 (ex. 게임 시작 or 설명 or 종료 등)
	m_TitleScreen = new UserInterface(Device, CommandList, m_RootSignature, T_TITLESCREEN);
	m_TitleScreen->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// Title State에서 플레이어가 어떤 행동을 할지 고르는 선택창
	m_Selection = new UserInterface(Device, CommandList, m_RootSignature, T_SELECTION);
	m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.05f, 0.f));

	m_GameManual = new UserInterface(Device, CommandList, m_RootSignature, T_GAMEMANUAL);
	m_GameManual->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// Camera를 가지고 있으며 플레이어가 직접 조종하는 오브젝트인 Player 생성
	m_Player = new Player(Device, CommandList, m_RootSignature);
	m_Player->SetScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_Player->SetPosition(DirectX::XMFLOAT3(2050.f, 0.f, 1150.f));

	// 각 정점 마다 높낮이가 다른 지형(Terrain) 생성
	m_Terrain = new Terrain(Device, CommandList, m_RootSignature);

	// 게임의 배경 역할을 하는 Skybox 생성
	m_Skybox = new MultipleTexture(Device, CommandList, m_RootSignature, T_SKYBOX);

	// Scene에 등장하는 빌보드 이미지를 생성 (ex. 나무)
	m_BillboardTree = new Billboard();
	m_BillboardTree->CreateShader(Device, m_RootSignature);
	m_BillboardTree->CreateBillboard(Device, CommandList, m_RootSignature, m_Terrain, T_BILLBOARDTREE, 625);

	// 플레이어가 맵 밖으로 이동하는 것을 방지하는 벽 생성
	m_Walls = new Billboard*[4];
	for (int i = 0; i < 4; ++i) {
		m_Walls[i] = new Billboard();
		m_Walls[i]->CreateShader(Device, m_RootSignature);
		m_Walls[i]->CreateBillboard(Device, CommandList, m_RootSignature, nullptr, T_WALL + i, 75);
	}

	// 플레이어와 거리가 가까워지는 빌보드 이미지를 객체로 변환하기 위해 빌보드 이미지의 Index 저장
	int BillTreeIndex = 0;
	for (int z = 0; z < 25; ++z) for (int x = 0; x < 25; ++x) m_SaveBillboardTreeIndex[x][z] = BillTreeIndex++;

	// 빌보드 이미지에서 변환되는 여러 개의 Mesh를 가진 Tree 생성
	int TreeCount = 9;
	m_Tree.reserve(TreeCount);
	for (int i = 0; i < TreeCount; ++i) m_Tree.emplace_back(new MultipleTexture(Device, CommandList, m_RootSignature, T_TREE));

	// HP 회복을 할 수 있는 아이템 생성
	m_ItemHp = new MultipleTexture(Device, CommandList, m_RootSignature, T_ITEMHP);
	m_ItemHp->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// 불꽃 효과를 표현하는 Flame 객체를 생성 - 1개의 불꽃 효과는 1개의 점 조명과 4개의 텍스처를 사용
	int FlameCount = 4 * 5;
	m_Flames.reserve(FlameCount);

	// 불꽃 효과의 월드 좌표(x, z)를 배열에 저장
	float FlamePos[10] = { 312.f, 4489.f, 2197.f, 4335.f, 1399.f, 3648.f, 228.f, 2805.f, 2348.f, 2773.f };
	int PosCount = -1;

	// FireArea Pos = x (0 - 2500), z (2500 - 5000)
	for (int i = 0; i < FlameCount; ++i) {
		m_Flames.emplace_back(new Effect(Device, CommandList, m_RootSignature, T_FLAME));

		// 1개의 불꽃 효과는 4개의 텍스처를 사용하므로 i % 4 == 0 일 때마다 월드 좌표 값을 변경
		if (i % 4 == 0) ++PosCount;
		else m_Flames.back()->SetRotate(DirectX::XMFLOAT3(0.f, 90.f * (i % 4), 0.f)); // 월드 좌표 값을 변경하지 않으면 텍스처를 y축 회전

		// 불꽃 효과의 월드 좌표를 설정
		float XPos = FlamePos[PosCount * 2], ZPos = FlamePos[(PosCount * 2) + 1];
		// 설정된 x, z좌표 값으로 Terrain Map에 맞는 월드 좌표 y를 설정
		int GetHeightMapX = int(XPos) / MAP_SCALE, GetHeightMapZ = int(ZPos) / MAP_SCALE;
		float GetHeightMapY = m_Terrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ) + 50.f;

		m_Flames.back()->SetPosition(DirectX::XMFLOAT3(XPos, GetHeightMapY, ZPos));
	}

	// 불꽃 효과와 함께 사용할 연기 생성 - 1개의 불꽃 효과는 3개의 연기 사용
	int SmokeCount = 3 * 5;
	m_Smokes.reserve(SmokeCount);
	PosCount = -4;

	for (int i = 0; i < SmokeCount; ++i) {
		m_Smokes.emplace_back(new Effect(Device, CommandList, m_RootSignature, T_SMOKE));

		if (i % 3 == 0) PosCount += 4;
		m_Smokes.back()->SetPosition(m_Flames[PosCount]->GetPosition());
	}

	// 화염 효과를 진압하는 소화기
	int PowderCount = 5;
	m_Powders.reserve(PowderCount);
	for (int i = 0; i < PowderCount; ++i) {
		m_Powders.emplace_back(new Effect(Device, CommandList, m_RootSignature, T_POWDER, i));
	}

	// 플레이어의 공격 시 발생하는 총의 불꽃
	m_Spark = new Effect(Device, CommandList, m_RootSignature, T_SPARK);

	// 몬스터가 플레이어에게 피격 됐을 때 발생하는 신호
	m_Signal = new Effect(Device, CommandList, m_RootSignature, T_SIGNAL);

	// 플레이어가 몬스터의 머리를 피격했을 때 발생
	m_Headshot = new Effect(Device, CommandList, m_RootSignature, T_HEADSHOT);
	m_Headshot->SetPosition(DirectX::XMFLOAT3(2050.f, 200.f, 1200.f));

	// 플레이어의 최대 체력을 나타내는 UI
	m_HpBar = new UserInterface(Device, CommandList, m_RootSignature, T_HPBAR);
	m_HpBar->SetPosition(DirectX::XMFLOAT3(-0.5f, 0.9f, 0.f));

	// 플레이어의 현재 체력을 나타내는 UI
	m_HpGauge = new UserInterface(Device, CommandList, m_RootSignature, T_HPGAUGE);
	m_HpGauge->SetPosition(DirectX::XMFLOAT3(0.f, 0.9f, 0.f));

	// 플레이어의 공격 목표를 결정하는 UI
	m_Aim = new UserInterface(Device, CommandList, m_RootSignature, T_AIM);
	m_Aim->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// 현재 남은 총알의 십의 자리 수
	m_Numbers = new UserInterface*[2];
	m_Numbers[0] = new UserInterface(Device, CommandList, m_RootSignature, T_NUMBERS);
	m_Numbers[0]->SetPosition(DirectX::XMFLOAT3(0.725f, -0.85f, 0.f));

	// 현재 남은 총알의 일의 자리 수
	m_Numbers[1] = new UserInterface(Device, CommandList, m_RootSignature, T_NUMBERS);
	m_Numbers[1]->SetPosition(DirectX::XMFLOAT3(0.85f, -0.85f, 0.f));

	// 플레이어가 사망했을 때 발생하는 화면
	m_GameOverScreen = new UserInterface(Device, CommandList, m_RootSignature, T_GAMEOVER);
	m_GameOverScreen->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// 게임 클리어 시에 발생하는 화면
	m_GameEndScreen = new UserInterface(Device, CommandList, m_RootSignature, T_GAMEEND);
	m_GameEndScreen->SetPosition(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

	// 플레이어가 Fire Area에 입장했을 때 발생하는 안내 문구
	m_EnterFire = new UserInterface(Device, CommandList, m_RootSignature, T_ENTERFIRE);
	m_EnterFire->SetPosition(DirectX::XMFLOAT3(-0.5f, -0.75f, 0.f));

	// 플레이어가 Monster Area에 입장했을 때 발생하는 안내 문구
	m_EnterMonster = new UserInterface(Device, CommandList, m_RootSignature, T_ENTERMONSTER);
	m_EnterMonster->SetPosition(DirectX::XMFLOAT3(-0.5f, -0.75f, 0.f));

	// 현재 플레이어의 위치가 어떤 Area인지 안내하는 문구
	m_GuideArea = new UserInterface(Device, CommandList, m_RootSignature, T_GUIDEAREA);
	m_GuideArea->SetPosition(DirectX::XMFLOAT3(0.375f, 0.875f, 0.f));

	// 현재 플레이어의 위치가 어떤 Area인지 안내하는 문구
	m_GuidePos = new UserInterface*[3];
	for (int i = 0; i < 3; ++i) {
		m_GuidePos[i] = new UserInterface(Device, CommandList, m_RootSignature, T_GUIDENONE + i);
		if (i == 2) m_GuidePos[i]->SetPosition(DirectX::XMFLOAT3(0.75f, 0.875f, 0.f));
		else m_GuidePos[i]->SetPosition(DirectX::XMFLOAT3(0.675f, 0.875f, 0.f));
	}

	// Fire, Monster Area 외의 지역에서 등장하는 일반 오크 몬스터
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

	// Monster Area( x (2500 - 5000), z (2500 - 5000) ) 에서 등장하는 몬스터들 (ex. Strong Orc, Shaman Orc, WolfRider Orc)
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

	// Monster Area에 등장하는 몬스터들의 색상을 어둡게 연출
	DirectX::XMFLOAT4 ChangeTexcoords = DirectX::XMFLOAT4(-1.f, -1.f, 0.f, 0.25f);
	for (int i = 0; i < m_StrongOrcs.size(); ++i) if (m_StrongOrcs[i] != nullptr) m_StrongOrcs[i]->SetChangeTexcoords(ChangeTexcoords);
	for (int i = 0; i < m_ShamanOrcs.size(); ++i) if (m_ShamanOrcs[i] != nullptr) m_ShamanOrcs[i]->SetChangeTexcoords(ChangeTexcoords);
	for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) if (m_WolfRiderOrcs[i] != nullptr) m_WolfRiderOrcs[i]->SetChangeTexcoords(ChangeTexcoords);
}

void Scene::UpdateConstantBuffer(ID3D12GraphicsCommandList* CommandList)
{
	memcpy(m_MappingLight->m_Lights, m_Lights, sizeof(LIGHT) * m_LightCount);
	memcpy(&(m_MappingLight->m_LightCount), &m_LightCount, sizeof(int));

	D3D12_GPU_VIRTUAL_ADDRESS LightGpuVirtualAddress = m_LightBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(2, LightGpuVirtualAddress);

	memcpy(&(m_MappingNoise->m_Noise), &m_Noise, sizeof(NOISE));
	D3D12_GPU_VIRTUAL_ADDRESS NoiseGpuVirtualAddress = m_NoiseBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(5, NoiseGpuVirtualAddress);

	memcpy(&(m_MappingDistortion->m_Distortion), &m_Distortion, sizeof(DISTORTION));
	D3D12_GPU_VIRTUAL_ADDRESS DistortionGpuVirtualAddress = m_DistortionBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(6, DistortionGpuVirtualAddress);
}

void Scene::UpdateNoneArea(float ElapsedTime)
{
	// 다른 지역의 오브젝트를 비활성화
	if (m_EnterFire != nullptr) m_EnterFire->Animate(0.f, 0);
	if (m_EnterMonster != nullptr) m_EnterMonster->Animate(0.f, 0);

	// Fire, Monster Area에서 변경된 오브젝트들의 텍스처 색상을 원래 색상으로 변경
	if (0.f <= m_ChangeColorTime) m_ChangeColorTime -= ElapsedTime * 0.125f;

	DirectX::XMFLOAT4 ChangeTexcoords = DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, m_ChangeColorTime);

	if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(ChangeTexcoords);
	if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(ChangeTexcoords);
	if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(ChangeTexcoords);
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(ChangeTexcoords);
	if (m_Player != nullptr) m_Player->SetChangeTexcoords(ChangeTexcoords);
}

void Scene::UpdateFireArea(float ElapsedTime)
{
	// 입장 문구 호출
	if (m_EnterFire != nullptr) m_EnterFire->Animate(ElapsedTime, 1);
	if (m_EnterMonster != nullptr) m_EnterMonster->Animate(0.f, 0);

	// Fire Area에 입장 시에 오브젝트들의 텍스처 색상을 어둡게 변경
	if (0.25f >= m_ChangeColorTime) m_ChangeColorTime += ElapsedTime * 0.125f;

	DirectX::XMFLOAT4 ChangeTexcoords = DirectX::XMFLOAT4(-1.f, -1.f, m_ChangeColorTime, 0.f);

	if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(ChangeTexcoords);
	if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(ChangeTexcoords);
	if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(ChangeTexcoords);
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(ChangeTexcoords);
	if (m_Player != nullptr) m_Player->SetChangeTexcoords(ChangeTexcoords);

	// 플레이어와 불꽃 효과 간의 거리를 구하기 위해 불꽃 효과의 월드 좌표 x, z를 가져옴
	float ToDistance[5]{};
	DirectX::XMFLOAT2 FlamePos[5]{};
	DirectX::XMFLOAT2 PlayerPos = DirectX::XMFLOAT2(m_Player->GetPosition().x, m_Player->GetPosition().z);
	for (int i = 0; i < 5; ++i) {
		FlamePos[i].x = m_Flames[i * 4]->GetPosition().x;
		FlamePos[i].y = m_Flames[i * 4]->GetPosition().z;

		ToDistance[i] = sqrt(((PlayerPos.x - FlamePos[i].x) * (PlayerPos.x - FlamePos[i].x)) + ((PlayerPos.y - FlamePos[i].y) * (PlayerPos.y - FlamePos[i].y)));
	}
	// 플레이어와 가장 가까운 화염 효과의 위치를 탐색
	int NearIndex = 0;
	float NearDistance = 0.f;
	for (int i = 0; i < 5; ++i) {
		if (i == 0) NearDistance = ToDistance[i], NearIndex = i;
		else {
			if (ToDistance[i] < NearDistance) NearDistance = ToDistance[i], NearIndex = i;
		}
	}
	// 플레이어와 가장 가까운 거리의 화염 효과를 점 조명의 위치로 설정
	m_Lights[1].m_Position = m_Flames[NearIndex * 4]->GetPosition();

	// 소화기 효과와 화염 효과의 충돌 처리
	DirectX::XMFLOAT3 NearFlamePos = DirectX::XMFLOAT3(m_Flames[NearIndex * 4]->GetPosition().x, m_Flames[NearIndex * 4]->GetPosition().y, m_Flames[NearIndex * 4]->GetPosition().z);
	DirectX::XMFLOAT2 PowderPos = DirectX::XMFLOAT2(m_Powders[0]->GetPosition().x, m_Powders[0]->GetPosition().z);
	float Distance = sqrt(((NearFlamePos.x - PowderPos.x) * (NearFlamePos.x - PowderPos.x)) + ((NearFlamePos.z - PowderPos.y) * (NearFlamePos.z - PowderPos.y)));

	if (25.f >= Distance && false == m_Flames[NearIndex * 4]->GetCollision()) for (int i = 0; i < 4; ++i) m_Flames[(NearIndex * 4) + i]->ActiveCollision();

	// 화염 효과의 크기가 감소한 만큼 충돌 거리와 조명 범위 감소
	int DecreaseRange = m_Flames[NearIndex * 4]->GetStackDecrease();
	m_Lights[1].m_Range = 250.f - (DecreaseRange * 50.f);

	// 플레이어와 화염 효과의 충돌 처리
	int CurrentAnimation = m_Player->GetCurrentAnimationTrackIndex();
	if (50.f - (DecreaseRange * 10.f) >= NearDistance && P_DAMAGED != CurrentAnimation) m_Player->ActiveDamaged(-1);

	for (int i = 0; i < m_Flames.size(); ++i) m_Flames[i]->Animate(ElapsedTime);

	// 화염 효과 진압 시에 HP 아이템 등장
	if (true == m_Flames[NearIndex * 4]->GetFlameOffSignal()) m_ItemHp->SetPosition(NearFlamePos), m_Flames[NearIndex * 4]->FalseFlameOffSignal();
	if (m_ItemHp != nullptr) m_ItemHp->Animate(ElapsedTime, NearFlamePos);

	DirectX::XMFLOAT2 ItemPos = DirectX::XMFLOAT2(m_ItemHp->GetPosition().x, m_ItemHp->GetPosition().z);
	Distance = sqrt(((ItemPos.x - PlayerPos.x) * (ItemPos.x - PlayerPos.x)) + ((ItemPos.y - PlayerPos.y) * (ItemPos.y - PlayerPos.y)));

	if (0.f == m_ItemHp->GetAnimateTime() && 10.f > Distance) m_ItemHp->ActiveAnimate(), m_Player->ActiveGetHpItem();
}

void Scene::UpdateMonsterArea(float ElapsedTime)
{
	// 입장 문구 호출
	if (m_EnterMonster != nullptr) m_EnterMonster->Animate(ElapsedTime, 1);
	if (m_EnterFire != nullptr) m_EnterFire->Animate(0.f, 0);

	// Monster Area에 입장 시에 오브젝트들의 텍스처 색상을 어둡게 변경
	if (0.25f >= m_ChangeColorTime) m_ChangeColorTime += ElapsedTime * 0.125f;

	DirectX::XMFLOAT4 ChangeTexcoords = DirectX::XMFLOAT4(-1.f, -1.f, 0.f, m_ChangeColorTime);

	// 3-1. Map Object
	if (m_Terrain != nullptr) m_Terrain->SetChangeTexcoords(ChangeTexcoords);
	if (m_Skybox != nullptr) m_Skybox->SetChangeTexcoords(ChangeTexcoords);
	if (m_BillboardTree != nullptr) m_BillboardTree->SetChangeTexcoords(ChangeTexcoords);
	for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->SetChangeTexcoords(ChangeTexcoords);
	if (m_Player != nullptr) m_Player->SetChangeTexcoords(ChangeTexcoords);
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
		float PlayerxPos = m_Player->GetPosition().x, PlayerzPos = m_Player->GetPosition().z;

		if (nullptr != m_Player) {
			// 1. Terrain의 Height Map에 따라 플레이어의 월드 좌표 y 좌표 설정
			int GetHeightMapX = int(PlayerxPos) / MAP_SCALE, GetHeightMapZ = int(PlayerzPos) / MAP_SCALE;
			float GetHeightMapY = m_Terrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ);

			// 2. 플레이어의 회전, 이동, 애니메이션을 수행 - 카메라 또한 플레이어와 함께 변경
			m_Player->Animate(ElapsedTime, Hwnd, m_PreviousPos, GetHeightMapY);
			m_Player->UpdateTransform(nullptr);

			// 3. 특정 애니메이션의 수행이 완료되면 초기화 및 비활성화 (ex. 장전 애니메이션 수행 - 총알 개수 최대, 애니메이션 비활성화)
			if (true == m_Player->GetCompletedReload()) m_BulletCount = 30, m_Player->SetCompletedReload(false);
		}

		// 현재 플레이어의 월드 좌표에 따라 지역을 활성화
		if ((PlayerxPos < 2500.f) && (PlayerzPos > 2400.f)) m_ActiveGuidePosIndex = AREA_FIRE;
		else if ((PlayerxPos >= 2500.f) && (PlayerzPos > 2400.f)) m_ActiveGuidePosIndex = AREA_MONSTER;
		else m_ActiveGuidePosIndex = AREA_NONE;

		// 활성화된 지역의 함수 수행 - 지역에 따라 오브젝트의 활성화 및 텍스처 색상 변경
		switch (m_ActiveGuidePosIndex) {
		case AREA_NONE:
		{
			UpdateNoneArea(ElapsedTime);
		}
		break;

		case AREA_FIRE:
		{
			UpdateFireArea(ElapsedTime);
		}
		break;

		case AREA_MONSTER:
		{
			UpdateMonsterArea(ElapsedTime);
		}
		break;
		}

		// 불꽃 효과를 표현하기 위해 Noise Buffer의 Frame Time을 증가
		m_Noise.m_FrameTime += ElapsedTime * 0.5f;
		if (m_Noise.m_FrameTime > 1000.f) m_Noise.m_FrameTime = 0.f;

		DirectX::XMFLOAT3 ShadowPos = m_Player->GetPosition();
		ShadowPos.y -= 20.f;
		m_Lights[2].m_Position = ShadowPos;

		// 몬스터 오브젝트들의 애니메이션 수행 - 플레이어 추격, 공격, 사망 등의 애니메이션과 상호작용 등
		bool PlayerHit = false;
		int HitMonsterKind = 0;

		for (int i = 0; i < m_WeakOrcs.size(); ++i)
			if (m_WeakOrcs[i] != nullptr) {
				m_WeakOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_WeakOrcs[i]->UpdateTransform(nullptr);

				if (true == m_WeakOrcs[i]->GetSuccessAttack()) PlayerHit = true, HitMonsterKind = M_WEAKORC, m_WeakOrcs[i]->SetSuccessAttack(false);

				// 몬스터 사망 시에 플레이어 강화
				if (true == m_WeakOrcs[i]->GetDeath()) m_WeakOrcs.erase(m_WeakOrcs.begin() + i), m_Player->SetPower(m_Player->GetPower() + 2);
			}

		for (int i = 0; i < m_StrongOrcs.size(); ++i)
			if (m_StrongOrcs[i] != nullptr) {
				m_StrongOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_StrongOrcs[i]->UpdateTransform(nullptr);

				if (true == m_StrongOrcs[i]->GetSuccessAttack()) PlayerHit = true, HitMonsterKind = M_STRONGORC, m_StrongOrcs[i]->SetSuccessAttack(false);
			}

		for (int i = 0; i < m_ShamanOrcs.size(); ++i)
			if (m_ShamanOrcs[i] != nullptr) {
				m_ShamanOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_ShamanOrcs[i]->UpdateTransform(nullptr);

				if (true == m_ShamanOrcs[i]->GetSuccessAttack()) PlayerHit = true, HitMonsterKind = M_SHAMANORC, m_ShamanOrcs[i]->SetSuccessAttack(false);
			}

		for (int i = 0; i < m_WolfRiderOrcs.size(); ++i)
			if (m_WolfRiderOrcs[i] != nullptr) {
				m_WolfRiderOrcs[i]->Animate(ElapsedTime, m_Player->GetTransformPos(), m_Terrain, m_Signal);
				m_WolfRiderOrcs[i]->UpdateTransform(nullptr);

				if (true == m_WolfRiderOrcs[i]->GetSuccessAttack()) PlayerHit = true, HitMonsterKind = M_WOLFRIDERORC, m_WolfRiderOrcs[i]->SetSuccessAttack(false);
				
				// 보스 몬스터의 사망 시에 게임 클리어 화면을 활성화
				if (true == m_WolfRiderOrcs[i]->GetDeath()) if (m_GameEndScreen != nullptr) m_GameEndScreen->Animate(ElapsedTime, 0);
			}

		if (true == PlayerHit) m_Player->ActiveDamaged(HitMonsterKind);

		// 플레이어와 근접한 빌보드 이미지를 여러 개의 메쉬를 가진 오브젝트로 변경
		DirectX::XMFLOAT4X4 *GetBillboardTreePos = m_BillboardTree->GetObjectsWorldPos();
		int NearX = int(PlayerxPos / 200), NearZ = int(PlayerzPos / 200);

		// 가장 근접한 나무를 기준으로 3x3개의 빌보드 이미지를 오브젝트로 변경
		for (int i = 0, RangeX = -1; RangeX < 2; ++RangeX) {
			for (int RangeZ = -1; RangeZ < 2; ++RangeZ, ++i) {
				int TreeXIndex = NearX + RangeX, TreeZIndex = NearZ + RangeZ;

				// 최소, 최대 범위 밖의 배열 접근 방지
				if (TreeXIndex < 0 || TreeXIndex > 24 || TreeZIndex < 0 || TreeZIndex > 24) continue;

				DirectX::XMFLOAT4X4 NearBillboardTree = GetBillboardTreePos[m_SaveBillboardTreeIndex[TreeXIndex][TreeZIndex]];

				if (m_Tree[i] != nullptr) m_Tree[i]->SetPosition(DirectX::XMFLOAT3(NearBillboardTree._41, NearBillboardTree._42, NearBillboardTree._43));
			}
		}

		// 카메라의 월드 좌표 Right, Up, Look에 따라 불꽃 효과와 연기 효과의 월드 좌표 설정
		Camera* GetCamera = m_Player->GetCamera();
		int PosCount = -4;

		for (int i = 0; i < m_Smokes.size(); ++i) {
			DirectX::XMFLOAT4X4 Target{};
			Target._11 = GetCamera->GetRight().x, Target._12 = GetCamera->GetRight().y, Target._13 = GetCamera->GetRight().z;
			Target._21 = GetCamera->GetUp().x, Target._22 = GetCamera->GetUp().y, Target._23 = GetCamera->GetUp().z;
			Target._31 = GetCamera->GetLook().x, Target._32 = GetCamera->GetLook().y, Target._33 = GetCamera->GetLook().z;

			if (i % 3 == 0) PosCount += 4;
			Target._41 = m_Flames[PosCount]->GetPosition().x, Target._42 = m_Flames[PosCount]->GetPosition().y, Target._43 = m_Flames[PosCount]->GetPosition().z;

			m_Smokes[i]->ActiveEffect(Target);
			m_Smokes[i]->Animate(ElapsedTime);
		}

		for (int i = 0; i < m_Powders.size(); ++i) { m_Powders[i]->SetTargetTransformPos(m_Player->GetTransformPos()), m_Powders[i]->Animate(ElapsedTime, i); }

		if (m_Spark != nullptr) m_Spark->Animate(ElapsedTime);
		if (m_Signal != nullptr) m_Signal->Animate(ElapsedTime);
		if (m_Headshot != nullptr) m_Headshot->Animate(ElapsedTime);

		if (m_HpGauge != nullptr) m_HpGauge->Animate(ElapsedTime, m_Player->GetHp());
		for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) m_Numbers[i]->Animate(i, m_BulletCount);
		if (P_DEATH == m_Player->GetCurrentAnimationTrackIndex()) if (m_GameOverScreen != nullptr) m_GameOverScreen->Animate(ElapsedTime, 0);

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

		if (m_GameManual != nullptr) m_GameManual->Render(CommandList);
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
		if (m_Spark != nullptr) m_Spark->Render(CommandList);
		if (m_Signal != nullptr) m_Signal->Render(CommandList);
		if (m_Headshot != nullptr) m_Headshot->Render(CommandList);

		// 3. Rendering Objects
		for (int i = 0; i < m_Flames.size(); ++i) m_Flames[i]->Render(CommandList);

		if (m_BillboardTree != nullptr) m_BillboardTree->Render(CommandList);
		for (int i = 0; i < m_Tree.size(); ++i) if (m_Tree[i] != nullptr) m_Tree[i]->Render(CommandList);
		if (m_ItemHp != nullptr) m_ItemHp->Render(CommandList);

		for (int i = 0; i < m_WeakOrcs.size(); ++i) if (m_WeakOrcs[i] != nullptr) m_WeakOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_StrongOrcs.size(); ++i) if (m_StrongOrcs[i] != nullptr) m_StrongOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_ShamanOrcs.size(); ++i) if (m_ShamanOrcs[i] != nullptr) m_ShamanOrcs[i]->Render(CommandList);
		for (int i = 0; i < m_WolfRiderOrcs.size(); ++i) if (m_WolfRiderOrcs[i] != nullptr) m_WolfRiderOrcs[i]->Render(CommandList);

		if (m_Player != nullptr) m_Player->Render(CommandList);

		// 4. Rendering UserInterface
		if (m_GameOverScreen != nullptr) m_GameOverScreen->Render(CommandList);
		if (m_GameEndScreen != nullptr) m_GameEndScreen->Render(CommandList);
		if (m_GameManual != nullptr) m_GameManual->Render(CommandList);
		if (m_HpBar != nullptr) m_HpBar->Render(CommandList);
		if (m_HpGauge != nullptr) m_HpGauge->Render(CommandList);
		if (m_Aim != nullptr) m_Aim->Render(CommandList);
		if (m_EnterFire != nullptr) m_EnterFire->Render(CommandList);
		if (m_EnterMonster != nullptr) m_EnterMonster->Render(CommandList);
		if (m_GuideArea != nullptr) m_GuideArea->Render(CommandList);
		if (m_GuidePos[m_ActiveGuidePosIndex] != nullptr) m_GuidePos[m_ActiveGuidePosIndex]->Render(CommandList);
		for (int i = 0; i < 2; ++i) if (m_Numbers[i] != nullptr) m_Numbers[i]->Render(CommandList);

		// 5. Rendering Blending Objects
		for (int i = 0; i < m_Smokes.size(); ++i) m_Smokes[i]->Render(CommandList);
		for (int i = 0; i < m_Powders.size(); ++i) m_Powders[i]->Render(CommandList);

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
				float GetSelectionY = m_Selection->GetPosition().y;
				if (-0.68f == GetSelectionY) m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.365f, 0.f));
				else m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.05f, 0.f));
			}
			break;

			case VK_DOWN:
			{
				float GetSelectionY = m_Selection->GetPosition().y;
				if (-0.05f == GetSelectionY) m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.365f, 0.f));
				else m_Selection->SetPosition(DirectX::XMFLOAT3(0.f, -0.68f, 0.f));
			}
			break;

			case VK_RETURN:
			{
				float GetSelectionY = m_Selection->GetPosition().y;
				if (-0.05f == GetSelectionY) { m_GameManual->SetActive(false), m_State = STATE_MAIN; }
				else if (-0.365f == GetSelectionY) { if (true == m_GameManual->GetActive()) m_GameManual->SetActive(false); else m_GameManual->SetActive(true); }
				else PostQuitMessage(0);
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
			{
				m_Player->ActiveMove(0, true);
			}
			break;

			case 's':
			case 'S':
			{
				m_Player->ActiveMove(1, true);
			}
			break;

			case 'a':
			case 'A':
			{
				m_Player->ActiveMove(2, true);
			}
			break;

			case 'd':
			case 'D':
			{
				m_Player->ActiveMove(3, true);
			}
			break;

			case 'r':
			case 'R':
			{
				if (m_BulletCount != 30) m_Player->ActiveReload();
			}
			break;

			case 'q':
			case 'Q':
			{
				if (m_GameManual != nullptr) m_GameManual->SetActive(true);
			}
			break;

			case VK_CONTROL:
			{
				m_ActivePowder = true;
			}
			break;

			case VK_SHIFT:
			{
				if (m_Player->GetCurrentAnimationTrackIndex() != P_DAMAGED) m_Player->ActiveRoll();
			}
			break;
			}
			break;

		case WM_KEYUP:
			switch (Wparam)
			{
			case 'w':
			case 'W':
			{
				m_Player->ActiveMove(0, false);
			}
			break;

			case 's':
			case 'S':
			{
				m_Player->ActiveMove(1, false);
			}
			break;

			case 'a':
			case 'A':
			{
				m_Player->ActiveMove(2, false);
			}
			break;

			case 'd':
			case 'D':
			{
				m_Player->ActiveMove(3, false);
			}
			break;

			case 'q':
			case 'Q':
			{
				if (m_GameManual != nullptr) m_GameManual->SetActive(false);
			}
			break;

			case VK_CONTROL:
			{
				m_ActivePowder = false;
			}
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

			// 플레이어 공격 처리 (ex. 총 발사 애니메이션 or 총 불꽃 활성화 or 몬스터 피격 여부 등)
			if (0 < m_BulletCount && m_Player->GetCurrentAnimationTrackIndex() < P_SHOOT && false == m_ActivePowder) {
				--m_BulletCount;
				m_Player->ActiveShoot();

				// 총구 불꽃 활성화
				GameObject* PlayerWeapon = m_Player->GetFrame(25);

				// Weapon의 모델 좌표를 저장
				DirectX::XMFLOAT4X4 WeaponPos = PlayerWeapon->GetTransformPos();

				// 플레이어의 현재 위치 정보를 통해 Weapon의 월드 좌표를 구함
				DirectX::XMFLOAT4X4 WeaponWorldPos{};
				DirectX::XMStoreFloat4x4(&WeaponWorldPos, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&WeaponPos), DirectX::XMLoadFloat4x4(&m_Player->GetWorldPos())));

				// Weapon의 월드 좌표를 넘겨줌
				if (m_Spark != nullptr) m_Spark->ActiveEffect(WeaponWorldPos);

				// 플레이어의 Look, Position 좌표를 이용하여 몬스터 오브젝트와 충돌처리 수행
				Camera* GetCamera = m_Player->GetCamera();
				DirectX::XMFLOAT3 StartPosition = GetCamera->GetPosition();
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
						// 몬스터의 충돌 거리가 최대 사거리보다 작으면 충돌 처리
						if (CollisionMonster->GetCollisionMeshDistance() <= MaxDistance) {
							// 충돌된 몬스터 중, 가장 가까운 거리의 몬스터만 충돌 처리를 수행
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
					// 헤드샷 여부에 따라 데미지 변경 및 효과 표시
					float ActiveHeadshot = false;
					int PlayerPower = m_Player->GetPower();
					if (strstr(ResultMonster->GetFrameName(), "Head")) {
						ActiveHeadshot = true;
						PlayerPower *= 2;
					}

					switch (MonsterType) {
					case M_WEAKORC:
					{
						m_WeakOrcs[CollisionMonsterIndex]->ActiveDamaged(PlayerPower);
						if (ActiveHeadshot) m_Headshot->ActiveEffect(m_WeakOrcs[CollisionMonsterIndex]->GetTransformPos());
					}
					break;

					case M_STRONGORC:
					{
						m_StrongOrcs[CollisionMonsterIndex]->ActiveDamaged(PlayerPower);
						if (ActiveHeadshot) m_Headshot->ActiveEffect(m_StrongOrcs[CollisionMonsterIndex]->GetTransformPos());
					}
					break;

					case M_SHAMANORC:
					{
						m_ShamanOrcs[CollisionMonsterIndex]->ActiveDamaged(PlayerPower);
						if (ActiveHeadshot) m_Headshot->ActiveEffect(m_ShamanOrcs[CollisionMonsterIndex]->GetTransformPos());
					}
					break;

					case M_WOLFRIDERORC:
					{
						m_WolfRiderOrcs[CollisionMonsterIndex]->ActiveDamaged(PlayerPower);
						if (ActiveHeadshot) m_Headshot->ActiveEffect(m_WolfRiderOrcs[CollisionMonsterIndex]->GetTransformPos());
					}
					break;
					}
				}
			}

			// 화염 효과를 진압하는 소화기 처리
			if (true == m_ActivePowder && P_SHOOT > m_Player->GetCurrentAnimationTrackIndex()) {
				m_Player->ActiveShoot();
				// 소화기 분말의 애니메이션 효과 활성화
				for (int i = 0; i < m_Powders.size(); ++i)
					m_Powders[i]->ActiveEffect(m_Player->GetTransformPos());
			}
		}
		break;
		}
	}
	break;
	}
}