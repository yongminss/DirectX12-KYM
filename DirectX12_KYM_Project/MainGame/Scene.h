#pragma once

// Scene에서 사용할 각 조명의 정보 (ex. 색상, 방향, 종류 등)
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

// 조명 버퍼에서 사용할 조명들 - 아직까진 햇빛으로 사용하는 방향성 조명 1개만 존재
struct MAPPING_LIGHT
{
	LIGHT m_Light;
};

class Player;
class Terrain;
class Skybox;
class UserInterface;
class GameObject;

// 렌더 타겟에 오브젝트가 렌더링 되며 실제 게임이 진행됨
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