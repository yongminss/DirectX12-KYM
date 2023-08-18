#pragma once

#define LIGHT_MAX 16

#define DIRECTIONAL_LIGHT 0
#define SPOT_LIGHT 1
#define POINT_LIGHT 2

// Scene에서 사용할 각 조명의 정보 (ex. 색상, 방향, 종류 등)
struct LIGHT
{
	DirectX::XMFLOAT4 m_Ambient;
	DirectX::XMFLOAT4 m_Diffuse;
	DirectX::XMFLOAT4 m_Specular;
	DirectX::XMFLOAT4 m_Emissive;
	DirectX::XMFLOAT3 m_Direction;
	float m_Range;
	DirectX::XMFLOAT3 m_Position;
	int m_Type;
	bool m_Active;
	DirectX::XMFLOAT3 m_Attenuation;
};

// 조명 버퍼에서 사용할 조명들
struct MAPPING_LIGHT
{
	LIGHT m_Lights[LIGHT_MAX];
	int m_LightCount;
};

// 불꽃 효과에 사용하는 Constant Buffer 생성 (ex. Noise Buffer, Distortion Buffer)
struct NOISE
{
	float m_FrameTime;
	DirectX::XMFLOAT3 m_ScrollSpeed;
	DirectX::XMFLOAT3 m_Scale;
};

struct MAPPING_NOISE
{
	NOISE m_Noise;
};

// Distortion Buffer
struct DISTORTION
{
	DirectX::XMFLOAT2 m_Distortion1;
	DirectX::XMFLOAT2 m_Distortion2;
	DirectX::XMFLOAT2 m_Distortion3;
	float m_Scale;
	float m_Bias;
};

struct MAPPING_DISTORTION
{
	DISTORTION m_Distortion;
};

class Texture;
class Player;
class Terrain;
class MultipleTexture;
class UserInterface;
class Billboard;
class Effect;
class Monster;

#define STATE_TITLE 0
#define STATE_MAIN 1

// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	static ID3D12DescriptorHeap *m_CbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CbvCpuDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_CbvGpuDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE m_SrvCpuDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_SrvGpuDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CbvCpuDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_CbvGpuDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE m_SrvCpuDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_SrvGpuDescriptorNextHandle;

	LIGHT *m_Lights = nullptr;
	int m_LightCount = 0;
	MAPPING_LIGHT *m_MappingLight = nullptr;
	ID3D12Resource *m_LightBuffer = nullptr;

	NOISE m_Noise{};
	MAPPING_NOISE *m_MappingNoise = nullptr;
	ID3D12Resource *m_NoiseBuffer = nullptr;

	DISTORTION m_Distortion{};
	MAPPING_DISTORTION *m_MappingDistortion = nullptr;
	ID3D12Resource *m_DistortionBuffer = nullptr;

	// Title State에서 사용하는 오브젝트
	UserInterface *m_TitleScreen = nullptr;
	UserInterface *m_Selection = nullptr;
	UserInterface *m_GameManual = nullptr;

	// Main State에서 사용하는 오브젝트
	Player *m_Player = nullptr;

	Terrain *m_Terrain = nullptr;
	MultipleTexture *m_Skybox = nullptr;
	Billboard *m_BillboardTree = nullptr;
	Billboard **m_Walls = nullptr;
	std::vector<MultipleTexture*> m_Tree{};
	MultipleTexture* m_ItemHp = nullptr;

	std::vector<Effect*> m_Flames{};
	std::vector<Effect*> m_Smokes{};
	std::vector<Effect*> m_Powders{};
	Effect *m_Spark = nullptr;
	Effect *m_Signal = nullptr;
	Effect *m_Headshot = nullptr;

	UserInterface *m_HpBar = nullptr;
	UserInterface *m_HpGauge = nullptr;
	UserInterface *m_Aim = nullptr;
	UserInterface **m_Numbers = nullptr;
	UserInterface *m_GameOverScreen = nullptr;
	UserInterface *m_GameEndScreen = nullptr;
	UserInterface *m_EnterFire = nullptr;
	UserInterface *m_EnterMonster = nullptr;
	UserInterface *m_GuideArea = nullptr;
	UserInterface **m_GuidePos = nullptr;

	std::vector<Monster*> m_WeakOrcs{};
	std::vector<Monster*> m_StrongOrcs{};
	std::vector<Monster*> m_ShamanOrcs{};
	std::vector<Monster*> m_WolfRiderOrcs{};

	POINT m_PreviousPos{};

	int m_State = STATE_TITLE;

	float m_ChangeColorTime = 0.f;
	int m_SaveBillboardTreeIndex[25][25]{};

	int m_BulletCount = 30;
	int m_ActiveGuidePosIndex = 0;
	bool m_ActiveGameManual = false;
	bool m_ActivePowder = false;
	int m_StackDecreaseFlame = 0;

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	
	static void CreateCbvSrvDescriptorHeap(ID3D12Device* Device, int ConstantBufferViewCount, int ShaderResourceViewCount);
	static void CreateShaderResourceView(ID3D12Device* Device, Texture* UsingTexture, int RootParameterIndex);

	void CreateConstantBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void UpdateConstantBuffer(ID3D12GraphicsCommandList* CommandList);
	void UpdateNoneArea(float ElapsedTime);
	void UpdateFireArea(float ElapsedTime);
	void UpdateMonsterArea(float ElapsedTime);
	void Animate(float ElapsedTime, HWND Hwnd);
	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam);
};