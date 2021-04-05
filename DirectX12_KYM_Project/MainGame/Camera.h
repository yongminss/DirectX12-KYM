#pragma once
// 플레이어가 보고 있는 화면을 렌더링하기 위한 좌표 등을 계산
class Camera
{
private:
	DirectX::XMFLOAT4X4 m_CameraPos{};
	DirectX::XMFLOAT4X4 m_ProjectionPos{};

	D3D12_VIEWPORT m_Viewport = { 0.f, 0.f, Window_Width, Window_Height, 0.f, 1.f };
	D3D12_RECT m_ScissorRect = { 0, 0, Window_Width, Window_Height };

public:
	Camera();
	~Camera();

	void CreateCamera();

	void SetViewportAndScissorRect(ID3D12GraphicsCommandList *CommandList);
	void UpdateShaderCode(ID3D12GraphicsCommandList* CommandList);
	void Update(ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Position);
};