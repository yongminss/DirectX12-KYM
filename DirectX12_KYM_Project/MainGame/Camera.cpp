#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::CreateCamera()
{
	XMStoreFloat4x4(&m_CameraPos, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixIdentity());

	DirectX::XMFLOAT3 position = { 0.f, 20.f, -20.f };
	DirectX::XMFLOAT3 look = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 up = { 0.f, 1.f, 0.f };

	DirectX::XMStoreFloat4x4(&m_CameraPos, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&look), DirectX::XMLoadFloat3(&up)));

	float AspectRatio = static_cast<float>(Window_Width) / static_cast<float>(Window_Height);
	DirectX::XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixPerspectiveFovLH(60.f, AspectRatio, 1.01f, 5000.f));
}

void Camera::SetViewportAndScissorRect(ID3D12GraphicsCommandList *CommandList)
{
	CommandList->RSSetViewports(1, &m_Viewport);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);
}

void Camera::UpdateShaderCode(ID3D12GraphicsCommandList* CommandList)
{
	DirectX::XMFLOAT4X4 CameraPos{};
	DirectX::XMStoreFloat4x4(&CameraPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_CameraPos)));

	CommandList->SetGraphicsRoot32BitConstants(0, 16, &CameraPos, 0);

	DirectX::XMFLOAT4X4 ProjectionPos{};
	DirectX::XMStoreFloat4x4(&ProjectionPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ProjectionPos)));

	CommandList->SetGraphicsRoot32BitConstants(0, 16, &ProjectionPos, 16);
}

void Camera::Update(ID3D12GraphicsCommandList* CommandList, DirectX::XMFLOAT3 Position)
{
	SetViewportAndScissorRect(CommandList);

	UpdateShaderCode(CommandList);
}