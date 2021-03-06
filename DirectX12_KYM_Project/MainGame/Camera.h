#pragma once

class Player;

// 플레이어가 보고 있는 화면을 렌더링하기 위한 좌표 등을 계산
class Camera
{
private:
	DirectX::XMFLOAT4X4 m_CameraPos{};
	DirectX::XMFLOAT4X4 m_ProjectionPos{};

	DirectX::XMFLOAT3 m_Offset = { 0.f, 20.f, -100.f };

	DirectX::XMFLOAT3 m_Right = { 1.f, 0.f, 0.f };
	DirectX::XMFLOAT3 m_Up = { 0.f, 1.f, 0.f };
	DirectX::XMFLOAT3 m_Look = { 0.f, 0.f, 1.f };
	DirectX::XMFLOAT3 m_Position{};

	D3D12_VIEWPORT m_Viewport = { 0.f, 0.f, Window_Width, Window_Height, 0.f, 1.f };
	D3D12_RECT m_ScissorRect = { 0, 0, Window_Width, Window_Height };

	float m_TimeLag = 0.f;

public:
	Camera();
	~Camera();

	void CreateCamera();

	void SetViewportAndScissorRect(ID3D12GraphicsCommandList *CommandList);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Update(ID3D12GraphicsCommandList* CommandList, float ElapsedTime, Player *Target);
};