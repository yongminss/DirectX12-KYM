#pragma once

// 카메라 버퍼(HLSL)에서 사용할 정보
struct MAPPING_CAMERA
{
	DirectX::XMFLOAT4X4 m_CameraPos;
	DirectX::XMFLOAT4X4 m_ProjectionPos;
	DirectX::XMFLOAT3 m_Position;
};

class Player;

// 플레이어가 보고 있는 화면을 렌더링하기 위한 좌표 등을 계산
class Camera
{
private:
	DirectX::XMFLOAT4X4 m_CameraPos{};
	DirectX::XMFLOAT4X4 m_ProjectionPos{};

	DirectX::XMFLOAT3 m_Offset = { 0.f, 25.f, -75.f };

	DirectX::XMFLOAT3 m_Right = { 1.f, 0.f, 0.f };
	DirectX::XMFLOAT3 m_Up = { 0.f, 1.f, 0.f };
	DirectX::XMFLOAT3 m_Look = { 0.f, 0.f, 1.f };
	DirectX::XMFLOAT3 m_Position{};

	D3D12_VIEWPORT m_Viewport = { 0.f, 0.f, Window_Width, Window_Height, 0.f, 1.f };
	D3D12_RECT m_ScissorRect = { 0, 0, Window_Width, Window_Height };

	float m_TimeLag = 0.f;

	MAPPING_CAMERA *m_MappingCamera = nullptr;
	ID3D12Resource *m_CameraBuffer = nullptr;

public:
	Camera();
	~Camera();

	void CreateCamera(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

	void SetOffset(DirectX::XMFLOAT3 Offset) { m_Offset = Offset; }

	DirectX::XMFLOAT3 GetRight() { return m_Right; }
	DirectX::XMFLOAT3 GetUp() { return m_Up; }
	DirectX::XMFLOAT3 GetLook() { return m_Look; }
	DirectX::XMFLOAT3 GetPosition() { return m_Position; }

	void SetViewportAndScissorRect(ID3D12GraphicsCommandList *CommandList);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Update(ID3D12GraphicsCommandList* CommandList, float ElapsedTime, Player *Target);
};