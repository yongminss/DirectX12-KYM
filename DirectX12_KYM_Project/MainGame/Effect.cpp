#include "stdafx.h"
#include "Effect.h"

#include "Mesh.h"
#include "Material.h"


Effect::Effect(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	switch (Kind) {
	case T_SIGNAL:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(3.5f, 10.5f, 0.f), 0);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;
	}
}

Effect::~Effect()
{

}

void Effect::Animate(float ElapsedTime)
{
	// 타입에 따른 효과를 처리 (ex. 플레이어의 총 발사 or 몬스터의 플레이어 인식 등) - 현재는 1개의 타입만 있으므로 향후 추가 예정
	if (m_Active) {
		if (0.f == m_AnimateTime) {
			m_TransformPos = m_TargetTransformPos;

			// Target의 Position을 이용하여 월드 좌표 계산
			DirectX::XMFLOAT3 Position{};
			DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), +5.f)));
			m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y + 80.f, m_TransformPos._43 = Position.z;

			m_TransformPos._11 = -m_TargetTransformPos._11, m_TransformPos._12 = -m_TargetTransformPos._12, m_TransformPos._13 = -m_TargetTransformPos._13;
			m_TransformPos._31 = -m_TargetTransformPos._31, m_TransformPos._32 = -m_TargetTransformPos._32, m_TransformPos._33 = -m_TargetTransformPos._33;

			UpdateTransform(nullptr);
		}
		m_AnimateTime += ElapsedTime;

		if (m_AnimateTime >= 0.5f) m_AnimateTime = 0.f, m_Active = false;
	}
}

void Effect::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (m_Active) GameObject::Render(CommandList);
}