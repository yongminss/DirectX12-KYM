#include "stdafx.h"
#include "Effect.h"

#include "Mesh.h"
#include "Material.h"


Effect::Effect(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, int Kind, int Index)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_Kind = Kind;

	switch (Kind) {
	case T_FLAME:
	{
		m_ActiveAnimate = true;
		m_ActiveRender = true;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(75.f, 50.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;

	case T_SMOKE:
	{
		m_ActiveAnimate = true;
		m_ActiveRender = true;

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(12.5f, 12.5f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;

	case T_POWDER:
	{
		m_AnimateTime = 0.f - (Index * 0.15f);

		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(10.f, 10.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;

	case T_SPARK:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(20.f, 20.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;

	case T_SIGNAL:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(3.5f, 10.5f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
		SetMesh(UsingMesh);

		Material *UsingMaterial = new Material();
		UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, Kind);
		SetMaterial(UsingMaterial);
	}
	break;

	case T_HEADSHOT:
	{
		TextureMesh *UsingMesh = new TextureMesh(Device, CommandList, DirectX::XMFLOAT3(40.f, 20.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f), Kind);
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

void Effect::Animate(float ElapsedTime, int Index)
{
	switch (m_Kind) {
	case T_FLAME:
	{
		if (true == m_ActiveAnimate) {
			if (m_Collision) {
				if (0.f == m_AnimateTime) {
					++m_StackDecrease;
					SetScale(DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f));
					float DecreaseYPos = m_PreviousYPos - (m_PreviousYPos * 0.75f);
					m_PreviousYPos -= DecreaseYPos;
					m_TransformPos._42 -= DecreaseYPos;
					// 화재 진압을 성공했으니 불꽃 효과 비활성화 및 신호 보냄
					if (4 < m_StackDecrease) m_ActiveAnimate = false, m_ActiveRender = false, m_FlameOffSignal = true;
				}
				m_AnimateTime += ElapsedTime;
				if (1.f < m_AnimateTime) m_AnimateTime = 0.f, m_Collision = false;
			}
		}
	}
	break;

	case T_SMOKE:
	{
		// 1개의 면을 사용하는 오브젝트이므로 카메라를 바라보도록 설정
		m_TransformPos._11 = m_TargetTransformPos._11, m_TransformPos._12 = m_TargetTransformPos._12, m_TransformPos._13 = m_TargetTransformPos._13;
		m_TransformPos._31 = m_TargetTransformPos._31, m_TransformPos._32 = m_TargetTransformPos._32, m_TransformPos._33 = m_TargetTransformPos._33;

		// 특정 범위를 넘어서면 초기화 - 속도와 x, z축은 다르게 설정
		float Distance = m_TransformPos._42 - m_TargetTransformPos._42;

		if (Distance > 125.f) {
			m_TransformPos._42 = (m_TargetTransformPos._42 - 25.f) + rand() % 40;
			m_TransformPos._41 = m_TargetTransformPos._41 + (-30.f + rand() % 60);
			m_TransformPos._43 = m_TargetTransformPos._43 + (-30.f + rand() % 60);
		}
		else {
			m_TransformPos._42 += (12.5f + rand() % 25) * ElapsedTime;
		}
	}
	break;

	case T_POWDER:
	{
		if (m_ActiveAnimate) {
			// 소화기 효과의 첫 호출 시에 월드 좌표를 설정
			if (!m_ActiveRender && 0.f <= m_AnimateTime) {
				m_ActiveRender = true;
				SetRight(DirectX::XMFLOAT3(m_TargetTransformPos._11, m_TargetTransformPos._12, m_TargetTransformPos._13));
				SetUp(DirectX::XMFLOAT3(m_TargetTransformPos._21, m_TargetTransformPos._22, m_TargetTransformPos._23));
				SetLook(DirectX::XMFLOAT3(m_TargetTransformPos._31, m_TargetTransformPos._32, m_TargetTransformPos._33));

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
			else {
				// 활성화된 소화기 효과의 애니메이션 수행
				DirectX::XMFLOAT3 Position{};
				DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetUp()), 10.f * ElapsedTime)));
				SetPosition(Position);

				DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), 100.f * ElapsedTime)));
				SetPosition(Position);

				m_CheckTime += ElapsedTime;
				if (0.05f < m_CheckTime) m_CheckTime = 0.f, SetScale(DirectX::XMFLOAT3(1.075f, 1.075f, 1.f));

				m_AnimateTime += ElapsedTime;
				if (1.5f < m_AnimateTime) m_TransformPos._41 = 10000.f, m_CheckTime = 0.f, m_AnimateTime = 0.f - (Index * 0.15f), m_ActiveAnimate = false, m_ActiveRender = false;
			}
		}
	}
	break;

	case T_SPARK:
	{
		if (m_ActiveAnimate) {
			if (0.f == m_AnimateTime) {
				m_ActiveRender = true;

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

			if (m_AnimateTime >= 0.025f) m_AnimateTime = 0.f, m_ActiveAnimate = false, m_ActiveRender = false;
		}
	}
	break;

	case T_SIGNAL:
	{
		if (m_ActiveAnimate) {
			if (0.f == m_AnimateTime) {
				m_ActiveRender = true;
				m_TransformPos = m_TargetTransformPos;

				// Target의 월드 좌표로 Signal의 월드 좌표 계산
				DirectX::XMFLOAT3 Position{};
				DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), +5.f)));
				m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y + 80.f, m_TransformPos._43 = Position.z;

				m_TransformPos._11 = -m_TargetTransformPos._11, m_TransformPos._12 = -m_TargetTransformPos._12, m_TransformPos._13 = -m_TargetTransformPos._13;
				m_TransformPos._31 = -m_TargetTransformPos._31, m_TransformPos._32 = -m_TargetTransformPos._32, m_TransformPos._33 = -m_TargetTransformPos._33;
			}
			m_AnimateTime += ElapsedTime;

			if (m_AnimateTime >= 0.5f) m_AnimateTime = 0.f, m_ActiveAnimate = false, m_ActiveRender = false;
		}
	}
	break;

	case T_HEADSHOT:
	{
		if (m_ActiveAnimate) {
			if (0.f == m_AnimateTime) {
				m_ActiveRender = true;
				m_TransformPos = m_TargetTransformPos;

				DirectX::XMFLOAT3 Position{};
				DirectX::XMStoreFloat3(&Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), +10.f)));
				m_TransformPos._41 = Position.x, m_TransformPos._42 = Position.y + 70.f, m_TransformPos._43 = Position.z;

				m_TransformPos._11 = -m_TargetTransformPos._11, m_TransformPos._12 = -m_TargetTransformPos._12, m_TransformPos._13 = -m_TargetTransformPos._13;
				m_TransformPos._31 = -m_TargetTransformPos._31, m_TransformPos._32 = -m_TargetTransformPos._32, m_TransformPos._33 = -m_TargetTransformPos._33;
			}
			m_AnimateTime += ElapsedTime;

			if (m_AnimateTime >= 0.5f) m_AnimateTime = 0.f, m_ActiveAnimate = false, m_ActiveRender = false;
		}
	}
	break;
	}

	UpdateTransform(nullptr);
}

void Effect::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (m_ActiveRender) GameObject::Render(CommandList);
}