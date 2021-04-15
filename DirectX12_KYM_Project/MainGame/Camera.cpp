#include "stdafx.h"
#include "Camera.h"
#include "Player.h"


Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::CreateCamera()
{
	DirectX::XMStoreFloat4x4(&m_CameraPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixIdentity());

	float AspectRatio = static_cast<float>(Window_Width) / static_cast<float>(Window_Height);
	
	DirectX::XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.f), AspectRatio, 1.01f, 5000.f));
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

void Camera::Update(ID3D12GraphicsCommandList* CommandList, Player *Target)
{
	SetViewportAndScissorRect(CommandList);

	// 1. �÷��̾� ȸ���� ���� ī�޶��� ȸ�� ������ �ٲٱ� ���� 4x4 ��� ����
	DirectX::XMFLOAT4X4 CameraRotate{};
	DirectX::XMStoreFloat4x4(&CameraRotate, DirectX::XMMatrixIdentity());

	// 2. �÷��̾��� ȸ�� ������ �������� ī�޶� ȸ�� ��Ŀ� ����
	DirectX::XMFLOAT3 Right = Target->GetRight();
	DirectX::XMFLOAT3 Up = Target->GetUp();
	DirectX::XMFLOAT3 Look = Target->GetLook();
	CameraRotate._11 = Right.x, CameraRotate._12 = Right.y, CameraRotate._13 = Right.z;
	CameraRotate._21 = Up.x, CameraRotate._22 = Up.y, CameraRotate._23 = Up.z;
	CameraRotate._31 = Look.x, CameraRotate._32 = Look.y, CameraRotate._33 = Look.z;

	// 3. ī�޶��� ������ ���͸� ȸ�� ��ķ� ��ȯ
	DirectX::XMFLOAT3 Offset{};
	DirectX::XMStoreFloat3(&Offset, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_Offset), DirectX::XMLoadFloat4x4(&CameraRotate)));

	// 4. �÷��̾��� ��ġ�� ȸ���� ������ ���͸� ���ؼ� ȸ���� ī�޶��� ��ġ�� ����
	DirectX::XMFLOAT3 Position{};
	DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Target->GetPosition()), DirectX::XMLoadFloat3(&Offset)));

	// 5. ���� ī�޶��� ��ġ���� ȸ���� ī�޶��� ��ġ������ ����� �Ÿ��� ��Ÿ���� ���͸� ����
	DirectX::XMFLOAT3 Direction{};
	DirectX::XMStoreFloat3(&Direction, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Position), DirectX::XMLoadFloat3(&m_Position)));

	// 6. ������ ���� ������ ���̸� ����
	float Length = 0.f;
	DirectX::XMFLOAT3 DirectionResult{};
	DirectX::XMStoreFloat3(&DirectionResult, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&Direction)));
	Length = DirectionResult.x;

	// 7. ������ ���̰� 0���� ũ�� ī�޶� ���� ��ġ���� ������ ���� ������ ũ�⸸ŭ ���Ͽ� ī�޶� �̵�
	if (Length > 0.f) {
		DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&Direction)));
	}

	// 8. ��ȯ�� ī�޶� ��ǥ�� HLSL�� ������ �� �ֵ��� ����
	m_CameraPos._11 = m_Right.x; m_CameraPos._12 = m_Up.x; m_CameraPos._13 = m_Look.x;
	m_CameraPos._21 = m_Right.y; m_CameraPos._22 = m_Up.y; m_CameraPos._23 = m_Look.y;
	m_CameraPos._31 = m_Right.z; m_CameraPos._32 = m_Up.z; m_CameraPos._33 = m_Look.z;
	DirectX::XMFLOAT3 NewPosition{};
	DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&m_Right)));
	m_CameraPos._41 = -NewPosition.x;
	DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&m_Up)));
	m_CameraPos._42 = -NewPosition.x;
	DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&m_Look)));
	m_CameraPos._43 = -NewPosition.x;

	UpdateShaderCode(CommandList);
}