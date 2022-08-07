#include "stdafx.h"
#include "Effect.h"

#include "Mesh.h"
#include "Material.h"


Effect::Effect(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_Kind = Kind;

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

	case T_SPARK:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(20.f, 20.f, 0.f), 0);
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
	// 타입에 따른 명령 처리 (ex. 몬스터의 플레이어 인식 or 플레이어의 공격 등)
	switch (m_Kind) {
	case T_SIGNAL: // 몬스터 피격 시의 효과
	{
		if (true == m_Active) {
			if (0.f == m_AnimateTime) {
				m_TransformPos = m_TargetTransformPos;

				// Target의 월드 좌표로 Signal의 월드 좌표 계산
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
	break;

	case T_SPARK: // 플레이어 공격 시의 효과
	{
		if (true == m_Active) {
			if (0.f == m_AnimateTime) {
				SetRight(DirectX::XMFLOAT3(m_TargetTransformPos._11, m_TargetTransformPos._12, m_TargetTransformPos._13));
				SetUp(DirectX::XMFLOAT3(m_TargetTransformPos._21, m_TargetTransformPos._22, m_TargetTransformPos._23));
				SetLook(DirectX::XMFLOAT3(m_TargetTransformPos._31, m_TargetTransformPos._32, m_TargetTransformPos._33));

				// Spark의 월드 좌표가 Weapon의 앞으로 오도록 설정
				DirectX::XMFLOAT3 Right{};
				DirectX::XMStoreFloat3(&Right, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetRight()), +12.f));

				DirectX::XMFLOAT3 Pos{};
				DirectX::XMStoreFloat3(&Pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_TargetTransformPos._41, m_TargetTransformPos._42, m_TargetTransformPos._43)), DirectX::XMLoadFloat3(&Right)));

				DirectX::XMFLOAT3 Look{};
				DirectX::XMStoreFloat3(&Look, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), +50.f));

				DirectX::XMStoreFloat3(&Pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Pos), DirectX::XMLoadFloat3(&Look)));

				Pos.y = m_TargetTransformPos._42 + 10.f;

				SetPosition(Pos);
			}
			m_AnimateTime += ElapsedTime;

			if (m_AnimateTime >= 0.025f) m_AnimateTime = 0.f, m_Active = false;
		}
	}
	break;
	}
}

void Effect::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (true == m_Active) GameObject::Render(CommandList);
}