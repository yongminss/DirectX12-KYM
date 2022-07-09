#include "stdafx.h"
#include "Camera.h"
#include "Player.h"


Camera::Camera()
{

}

Camera::~Camera()
{
	if (m_CameraBuffer != nullptr) m_CameraBuffer->Release();
}

void Camera::CreateCamera(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	DirectX::XMStoreFloat4x4(&m_CameraPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixIdentity());

	float AspectRatio = static_cast<float>(Window_Width) / static_cast<float>(Window_Height);

	DirectX::XMStoreFloat4x4(&m_ProjectionPos, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.f), AspectRatio, 1.01f, 5000.f));

	m_TimeLag = 0.25f;

	UINT BufferSize = ((sizeof(MAPPING_CAMERA) + 255) & ~255);
	m_CameraBuffer = CreateBuffer(Device, CommandList, nullptr, BufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	m_CameraBuffer->Map(0, nullptr, (void**)&m_MappingCamera);
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

	memcpy(&m_MappingCamera->m_CameraPos, &CameraPos, sizeof(DirectX::XMFLOAT4X4));

	DirectX::XMFLOAT4X4 ProjectionPos{};
	DirectX::XMStoreFloat4x4(&ProjectionPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ProjectionPos)));

	memcpy(&m_MappingCamera->m_ProjectionPos, &ProjectionPos, sizeof(DirectX::XMFLOAT4X4));

	memcpy(&m_MappingCamera->m_Position, &m_Position, sizeof(DirectX::XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS CameraGpuVirtualAddress = m_CameraBuffer->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView(0, CameraGpuVirtualAddress);
}

void Camera::Update(ID3D12GraphicsCommandList* CommandList, float ElapsedTime, Player *Target)
{
	SetViewportAndScissorRect(CommandList);

	// 1. �÷��̾� ȸ���� ���� ī�޶��� ȸ�� ������ �ٲٱ� ���� 4x4 ��� ����
	DirectX::XMFLOAT4X4 CameraRotate{};
	DirectX::XMStoreFloat4x4(&CameraRotate, DirectX::XMMatrixIdentity());

	// 2. �÷��̾��� ȸ�� ������ �������� ī�޶� ȸ�� ��Ŀ� ����
	DirectX::XMFLOAT3 Right{}, Up{}, Look{};
	Right = Target->GetCameraRight(), Up = Target->GetCameraUp(), Look = Target->GetCameraLook();
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

	// 6. ������ ���� ������ ���̸� ���ϰ�, ī�޶� �÷��̾�� ���� �ʰ� �̵��ϵ��� TimeLeg�� ����
	float Length = 0.f;
	DirectX::XMFLOAT3 DirectionResult{};
	DirectX::XMStoreFloat3(&DirectionResult, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&Direction)));
	Length = DirectionResult.x;

	DirectX::XMStoreFloat3(&Direction, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Direction)));

	float TimeLagScale = ElapsedTime * (1.f / m_TimeLag);
	float Distance = Length * TimeLagScale;

	// 7. ������ ���̰� 0���� ũ�� ī�޶� ���� ��ġ���� ������ ���� ������ ũ�⸸ŭ ���Ͽ� ī�޶� �̵�
	if (Distance > 0.f) {
		DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_Position), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Direction), Distance)));

		DirectX::XMFLOAT3 CameraShift{};
		DirectX::XMStoreFloat3(&CameraShift, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&CameraShift), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Look), 1000.f)));

		DirectX::XMFLOAT3 LookAt = Target->GetPosition();
		DirectX::XMStoreFloat3(&LookAt, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&LookAt), DirectX::XMLoadFloat3(&CameraShift)));

		// 8. �÷��̾� ȸ�� ������ ���� ī�޶��� ȸ�� ������ ����
		DirectX::XMFLOAT4X4 RotateInfo{};
		DirectX::XMStoreFloat4x4(&RotateInfo, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&LookAt), DirectX::XMLoadFloat3(&Target->GetCameraUp())));

		m_Right = DirectX::XMFLOAT3(RotateInfo._11, RotateInfo._21, RotateInfo._31);
		m_Up = DirectX::XMFLOAT3(RotateInfo._12, RotateInfo._22, RotateInfo._32);
		m_Look = DirectX::XMFLOAT3(RotateInfo._13, RotateInfo._23, RotateInfo._33);
	}

	// 9. ��ȯ�� ī�޶� ��ǥ�� HLSL�� ������ �� �ֵ��� ����
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