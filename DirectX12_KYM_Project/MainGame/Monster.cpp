#include "stdafx.h"
#include "Monster.h"

#include "Terrain.h"
#include "Effect.h"


Monster::Monster(int Type) : m_Type(Type)
{

}

Monster::~Monster()
{

}

void Monster::MoveToPlayer(float ElapsedTime, DirectX::XMFLOAT4X4 TargetTransformPos, Effect* Signal)
{
	DirectX::XMFLOAT3 TargetPosition = DirectX::XMFLOAT3(TargetTransformPos._41, TargetTransformPos._42, TargetTransformPos._43);
	TargetPosition.y -= 25.f; // TargetPosition�� Root Frame�̹Ƿ� �÷��̾� ���� ������ �������� ����

	// ���Ϳ� �÷��̾��� �Ÿ��� ���� ������ Animate && Render�� ����
	DirectX::XMFLOAT3 CalDistance{};
	DirectX::XMStoreFloat3((&CalDistance), DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition()))));

	float Distance = CalDistance.x;

	if ((1000.f * 1000.f) >= (Distance * Distance) && false == m_Death) {
		m_ActiveMonster = true;
	}
	else m_ActiveMonster = false;

	if (true == m_ActiveMonster) {
		// ���Ͱ� �ǰ� ������ �� �ǰ� ȿ�� (ex. �������� ���� ��ȭ && �ǰ� �ִϸ��̼�)
		if (true == m_ActiveDamaged && m_State != 3) {
			if (0.f == m_HitTime) {
				m_State = 0;
				SetDamaged(0x02); // �ǰݵ� ���� ���������� �����ֱ� ���� ������ ����

				// ù �ǰ��� ��� Signal�� �̿��Ͽ� ���Ͱ� �÷��̾ �ν��ߴٴ� ���� ������
				if (false == m_FirstHit) {
					Signal->ActiveEffect(m_TransformPos), m_FirstHit = true;
				}

				// ������ �ǰ� �ִϸ��̼� ���� - Shaman�� �ִϸ��̼��� �ε����� �ٸ��Ƿ� Ÿ�Կ� ���� ����
				switch (m_Type) {
				case 2:
				{
					if (GetCurrentAnimationTrackIndex() != M_DAMAGED - 1) {
						SetAnimationTrack(M_DAMAGED - 1, ANIMATION_TYPE_ONCE);
						m_Hp -= 10;
					}
				}
				break;

				default:
				{
					if (GetCurrentAnimationTrackIndex() != M_DAMAGED) {
						SetAnimationTrack(M_DAMAGED, ANIMATION_TYPE_ONCE);
						m_Hp -= 10;
					}
				}
				break;
				}
			}
			m_HitTime += ElapsedTime;

			// ���� �ð��� ������ ���� �������� ���ƿ�
			if (m_HitTime >= 0.15f) {
				SetDamaged(0x00);
			}

			// �ǰ� �ִϸ��̼��� �ð� 0.6667�ʰ� ������ ������ ���� �ൿ�� ����
			if (m_HitTime >= 0.6f) {
				m_State = 1;
				m_ActiveDamaged = false, m_HitTime = 0.f;
			}
		}
		// �ǰ� ���� ���� �ൿ�� ����
		else {
			if (0.f >= m_Hp) m_State = 3;

			// State ���� ���� ������ �ൿ ���� - 0 (�÷��̾� �ν�), 1 (�÷��̾� �߰�), 2 (�÷��̾� ����) 3 (���� ���)
			switch (m_State) {
			case 0: // Ready State
			{
				// 1. �÷��̾��� ���� ��ǥ���� ������ ���� ��ǥ�� �����Ͽ� ������ ���� ���͸� ���
				DirectX::XMFLOAT3 NewLook{};
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
				// ����ȭ
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

				// 2. ���ο� Look ���͸� ��������Ƿ� Right ���͵� ����
				DirectX::XMFLOAT3 NewRight{};
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
				// ����ȭ
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

				// 3. ���� Right, Look ���͸� ����
				SetRight(NewRight), SetLook(NewLook);

				// �÷��̾�� ���� ���� �Ÿ��� �ʹ� ��������� ���͸� �ڷ� �̵�
				if ((80.f * 80.f) >= (Distance * Distance)) {
					if (GetCurrentAnimationTrackIndex() != M_WALK) SetAnimationTrack(M_WALK, ANIMATION_TYPE_ONCE);

					DirectX::XMFLOAT3 NewPosition{};
					DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), -ElapsedTime * 100.f)));

					SetPosition(NewPosition);
				}
			}
			break;

			case 1: // Tracking State
			{
				// ���� �������� �÷��̾��� �Ÿ��� �� ���, �÷��̾ �߰�
				if ((60.f * 60.f) <= (Distance * Distance)) {
					SetAnimationTrack(M_RUN, ANIMATION_TYPE_LOOP);

					DirectX::XMFLOAT3 NewLook{};
					DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
					DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

					DirectX::XMFLOAT3 NewRight{};
					DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
					DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

					SetRight(NewRight), SetLook(NewLook);

					// ���Ͱ� �÷��̾ �ִ� �������� �̵�
					DirectX::XMFLOAT3 NewPosition{};
					DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), ElapsedTime * 100.f)));

					SetPosition(NewPosition);
				}
				// �Ÿ��� ��������� �÷��̾ ����
				else m_State = 2;
			}
			break;

			case 2: // Attack State
			{
				if (GetCurrentAnimationTrackIndex() != M_ATTACK_A) {
					if ((30.f * 30.f) <= (Distance * Distance)) {
						DirectX::XMFLOAT3 NewLook{};
						DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
						DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

						DirectX::XMFLOAT3 NewRight{};
						DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
						DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

						SetRight(NewRight), SetLook(NewLook);
					}
					SetAnimationTrack(M_ATTACK_A, ANIMATION_TYPE_ONCE);
				}

				m_AnimateTime += ElapsedTime;

				// Ÿ�� ���� �ִϸ��̼��� �ð��� �ٸ��Ƿ� �з�
				switch (m_Type) {
				case 0:
				case 1:
				{
					// ���⸦ �ֵѷ��� ��, �÷��̾ ���� ���� ������ ���� ����ó��
					if (false == m_SuccessAttack && m_AnimateTime > 0.3f && (60.f * 60.f) > (Distance * Distance)) m_SuccessAttack = true;

					// Weak & Strong : A (1.0), B (1.0)
					if (m_AnimateTime >= 0.999f) m_AnimateTime = 0.f, m_State = 1;
				}
				break;

				default:
				{
					// Shaman : 1.33, WolfRider : A (1.3334), B (1.3334)
					if (m_AnimateTime >= 1.329f) m_AnimateTime = 0.f, m_State = 1;

					// ���⸦ �ֵѷ��� ��, �÷��̾ ���� ���� ������ ���� ����ó��
					if (false == m_SuccessAttack && m_AnimateTime > 0.3f && (60.f * 60.f) > (Distance * Distance)) m_SuccessAttack = true;

					// Shaman : 1.33, WolfRider : A (1.3334), B (1.3334)
					if (m_AnimateTime >= 1.3333) m_AnimateTime = 0.f, m_State = 1;
				}
				break;
				}
			}
			break;

			case 3: // Death State
			{
				if (GetCurrentAnimationTrackIndex() != M_DEATH_A) SetAnimationTrack(M_DEATH_A, ANIMATION_TYPE_ONCE);

				m_AnimateTime += ElapsedTime;

				switch (m_Type) {
				case 0:
				case 1:
				{
					// Weak & Strong : A (1.83334), B (1.6667)
					if (m_AnimateTime >= 1.799f) m_AnimateTime = 0.f, m_Death = true;
				}
				break;

				case 2:
				{
					// Shaman : A (1.6667), B (1.6667)
					if (m_AnimateTime >= 1.599f) m_AnimateTime = 0.f, m_Death = true;
				}
				break;

				default:
				{
					// WolfRider : A (2.000001) B (2.0)
					if (m_AnimateTime >= 1.949f) m_AnimateTime = 0.f, m_Death = true;
				}
				break;
				}
			}
			break;
			}
		}
	}
}

void Monster::Animate(float ElapsedTime, DirectX::XMFLOAT4X4 TargetTransformPos, Terrain* GetTerrain, Effect* Signal)
{
	// 1. ��ɿ� ���� ������ �ൿ ����
	MoveToPlayer(ElapsedTime, TargetTransformPos, Signal);

	// ���Ͱ� Ȱ�� �����϶��� Animate && Render
	if (true == m_ActiveMonster) {
		// 2. Scene�� �ִ� HeightMap�� ���̿� ���� ������ y��ǥ�� ����
		int GetHeightMapX = int(m_TransformPos._41) / MAP_SCALE, GetHeightMapZ = int(m_TransformPos._43) / MAP_SCALE;
		float GetHeightMapY = GetTerrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ);

		SetPosition(DirectX::XMFLOAT3(m_TransformPos._41, GetHeightMapY, m_TransformPos._43));

		// 3. ������ ������ ������ �ִϸ��̼��� ����
		GameObject::Animate(ElapsedTime);
	}
}

void Monster::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (true == m_ActiveMonster) GameObject::Render(CommandList);
}