#pragma once

// Scene���� ����� �� ������ ���� (ex. ����, ����, ���� ��)
struct LIGHT
{
	DirectX::XMFLOAT4 m_Ambient;
	DirectX::XMFLOAT4 m_Diffuse;
	DirectX::XMFLOAT4 m_Specular;
	DirectX::XMFLOAT4 m_Emissive;
	DirectX::XMFLOAT3 m_Direction;
	bool m_Active;
	int m_Type;
};

// ���� ���ۿ��� ����� ����� - �������� �޺����� ����ϴ� ���⼺ ���� 1���� ����
struct MAPPING_LIGHT
{
	LIGHT m_Light;
};

class Player;
class Terrain;
class Skybox;
class UserInterface;
class GameObject;

// ���� Ÿ�ٿ� ������Ʈ�� ������ �Ǹ� ���� ������ �����
class Scene
{
private:
	ID3D12RootSignature *m_RootSignature = nullptr;

	LIGHT m_Light{};
	MAPPING_LIGHT *m_MappingLight = nullptr;
	ID3D12Resource *m_LightBuffer = nullptr;

	Player *m_Player = nullptr;
	Terrain *m_Terrain = nullptr;
	Skybox *m_Skybox = nullptr;
	UserInterface *m_HpBar = nullptr;
	UserInterface *m_HpGauge = nullptr;
	std::vector<GameObject*> m_GameObjects{};

	POINT m_PreviousPos{};

public:
	Scene();
	~Scene();

	void CreateRootSignature(ID3D12Device* Device);
	void CreateLightShaderBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void UpdateLightShaderBuffer(ID3D12GraphicsCommandList* CommandList);
	void Animate(float ElapsedTime, HWND Hwnd);
	void Render(ID3D12GraphicsCommandList* CommandList);

	void KeyboardMessage(UINT MessageIndex, WPARAM Wparam);
	void MouseMessage(HWND Hwnd, UINT MessageIndex, LPARAM Lparam);
};