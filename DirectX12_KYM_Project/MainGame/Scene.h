#pragma once

// Scene���� ����� �� ������ ���� (ex. ����, ����, ���� ��)
struct LIGHT
{
	DirectX::XMFLOAT4 m_Ambient;
	DirectX::XMFLOAT4 m_Diffuse;
	DirectX::XMFLOAT4 m_Specular;
	DirectX::XMFLOAT4 m_Emissive;
	DirectX::XMFLOAT3 m_Direction;
	int m_Type;
	bool m_Active;
};

// ���� ���ۿ��� ����� ����� - �������� �޺����� ����ϴ� ���⼺ ���� 1���� ����
struct MAPPING_LIGHT
{
	LIGHT m_Light;
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

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	int m_State = STATE_TITLE;

	ID3D12RootSignature *m_RootSignature = nullptr;

	static ID3D12DescriptorHeap *m_CbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorNextHandle;

	LIGHT m_Light{};
	MAPPING_LIGHT *m_MappingLight = nullptr;
	ID3D12Resource *m_LightBuffer = nullptr;

	// Title State���� ����ϴ� ������Ʈ
	UserInterface *m_TitleScreen = nullptr;
	UserInterface *m_Selection = nullptr;

	// Main State���� ����ϴ� ������Ʈ
	Player *m_Player = nullptr;
	Terrain *m_Terrain = nullptr;
	MultipleTexture *m_Skybox = nullptr;
	std::vector<MultipleTexture*> m_Tree{};
	UserInterface *m_HpBar = nullptr;
	UserInterface *m_HpGauge = nullptr;
	UserInterface *m_Aim = nullptr;
	UserInterface *m_GameOverScreen = nullptr;
	std::vector<UserInterface*> m_Numbers{};
	Billboard *m_BillboardGrass = nullptr;
	Billboard *m_BillboardTree = nullptr;
	Effect *m_Signal = nullptr;
	Effect *m_Spark = nullptr;
	std::vector<Monster*> m_WeakOrcs{};
	std::vector<Monster*> m_StrongOrcs{};
	std::vector<Monster*> m_ShamanOrcs{};
	std::vector<Monster*> m_WolfRiderOrcs{};

	POINT m_PreviousPos{};

	int m_BulletCountOne = 0;
	int m_BulletCountTen = 3;
	int m_BulletCount = 30;

	int m_SaveBillboardTreeIndex[25][25]{};

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	
	static void CreateCbvSrvDescriptorHeap(ID3D12Device* Device, int ConstantBufferViewCount, int ShaderResourceViewCount);
	static void CreateShaderResourceView(ID3D12Device* Device, Texture* UsingTexture, int RootParameterIndex);

	void CreateLightShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void UpdateLightShaderBuffer(ID3D12GraphicsCommandList* CommandList);
	void Animate(float ElapsedTime, HWND Hwnd);
	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam);
};