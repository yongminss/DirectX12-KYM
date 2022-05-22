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
class Skybox;
class UserInterface;
class Billboard;
class Effect;
class Monster;

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	static ID3D12DescriptorHeap *m_CbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorNextHandle;

	LIGHT m_Light{};
	MAPPING_LIGHT *m_MappingLight = nullptr;
	ID3D12Resource *m_LightBuffer = nullptr;

	Player *m_Player = nullptr;
	Terrain *m_Terrain = nullptr;
	Skybox *m_Skybox = nullptr;
	UserInterface *m_HpBar = nullptr;
	UserInterface *m_HpGauge = nullptr;
	Billboard *m_Grass = nullptr;
	Billboard *m_Tree = nullptr;
	Effect *m_Signal = nullptr;
	std::vector<Monster*> m_WeakOrcs{};
	std::vector<Monster*> m_StrongOrcs{};
	std::vector<Monster*> m_ShamanOrcs{};
	std::vector<Monster*> m_WolfRiderOrcs{};

	POINT m_PreviousPos{};

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