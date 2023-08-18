#include "stdafx.h"
#include "Monster.h"

#include "Terrain.h"
#include "Effect.h"


enum MONSTER_STATE
{
	READY,
	TRACKING,
	ATTACK,
	DAMAGED,
	DEATH
};


Monster::Monster(int Type) : m_Type(Type)
{
	// Monster Type�� ���� �ɷ�ġ�� ����
	switch (m_Type) {
	case M_WEAKORC:
	{
		m_Hp = 50;
	}
	break;

	case M_STRONGORC:
	{
		m_Hp = 100;
	}
	break;

	case M_SHAMANORC:
	{
		m_Hp = 50;
	}
	break;

	case M_WOLFRIDERORC:
	{
		m_Hp = 1000;
	}
	break;
	}
}

Monster::~Monster()
{

}

void Monster::MoveToPlayer(float ElapsedTime, DirectX::XMFLOAT4X4 TargetTransformPos, Effect* Signal)
{
	// �÷��̾��� ������ Ÿ�� ������Ʈ�� ����
	DirectX::XMFLOAT3 TargetPosition = DirectX::XMFLOAT3(TargetTransformPos._41, TargetTransformPos._42, TargetTransformPos._43);
	TargetPosition.y -= 25.f;

	// Ÿ�ٰ��� �Ÿ��� ���� Animate, Render�� Ȱ��ȭ ���θ� ����
	DirectX::XMFLOAT3 CalDistance{};
	DirectX::XMStoreFloat3((&CalDistance), DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition()))));

	float Distance = CalDistance.x;

	if (1000.f * 1000.f >= Distance * Distance) m_ActiveMonster = true;
	else m_ActiveMonster = false;

	if (m_Death) m_ActiveMonster = false;

	// Ÿ�ٰ� �Ÿ��� ����� ���� ������Ʈ Ȱ��ȭ
	if (m_ActiveMonster) {
		if (0 >= m_Hp) m_State = DEATH;
		if (m_State != DAMAGED) SetChangeTexcoords(DirectX::XMFLOAT4(0.f, -1.f, -1.f, -1.f));
		if (m_State != DEATH && m_ActiveDamaged) m_State = DAMAGED, m_AnimateTime = 0.f;

		// ������ ���¿� ���� �ൿ ���� ����
		switch (m_State) {
		case READY:
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

		case TRACKING:
		{
			// ���� ���� ���� �̵��ϱ� ���� �÷��̾ �߰�
			if ((60.f * 60.f) <= (Distance * Distance)) {
				SetAnimationTrack(M_RUN, ANIMATION_TYPE_LOOP);

				DirectX::XMFLOAT3 NewLook{};
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

				DirectX::XMFLOAT3 NewRight{};
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

				SetRight(NewRight), SetLook(NewLook);

				DirectX::XMFLOAT3 NewPosition{};
				DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), ElapsedTime * 100.f)));

				SetPosition(NewPosition);
			}
			// ���� ���� �̵��ϸ� ���� ���
			else m_State = ATTACK, m_AnimateTime = 0.f;
		}
		break;

		case ATTACK:
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

			// Ÿ�� ���� �ִϸ��̼��� �ð��� �ٸ��Ƿ� �з�
			switch (m_Type) {
			case M_WEAKORC:
			case M_STRONGORC:
			{
				// ���⸦ �ֵѷ��� ��, �÷��̾ ���� ���� ������ ���� ����ó��
				if (false == m_SuccessAttack && m_AnimateTime > 0.5f && (60.f * 60.f) > (Distance * Distance)) m_SuccessAttack = true;

				// Weak & Strong : A (1.0), B (1.0)
				if (m_AnimateTime >= 0.999f) m_AnimateTime = 0.f, m_State = TRACKING;
			}
			break;

			default:
			{
				// ���⸦ �ֵѷ��� ��, �÷��̾ ���� ���� ������ ���� ����ó��
				if (false == m_SuccessAttack && m_AnimateTime > 0.5f && (60.f * 60.f) > (Distance * Distance)) m_SuccessAttack = true;

				// Shaman : 1.33, WolfRider : A (1.3334), B (1.3334)
				if (m_AnimateTime >= 1.329f) m_AnimateTime = 0.f, m_State = TRACKING;
			}
			break;
			}
			m_AnimateTime += ElapsedTime;
		}
		break;

		case DAMAGED:
		{
			m_ActiveDamaged = false;

			// ù ��° ������ ���ϸ� ��ȣ ȿ�� �߻�
			if (!m_FirstHit) Signal->ActiveEffect(m_TransformPos), m_FirstHit = true;

			// �÷��̾�� ���� ���ϸ� �ؽ�ó ������ �Ͻ������� ����
			if (0.15f >= m_AnimateTime) SetChangeTexcoords(DirectX::XMFLOAT4(1.f, -1.f, -1.f, -1.f));
			else SetChangeTexcoords(DirectX::XMFLOAT4(0.f, -1.f, -1.f, -1.f));

			switch (m_Type) {
			case M_SHAMANORC:
			{
				if (GetCurrentAnimationTrackIndex() != M_DAMAGED - 1) {
					SetAnimationTrack(M_DAMAGED - 1, ANIMATION_TYPE_ONCE);
					m_Hp -= m_PlayerPower;
				}
			}
			break;

			default:
			{
				if (GetCurrentAnimationTrackIndex() != M_DAMAGED) {
					SetAnimationTrack(M_DAMAGED, ANIMATION_TYPE_ONCE);
					m_Hp -= m_PlayerPower;
				}
			}
			break;
			}

			if (0.6f <= m_AnimateTime) m_State = TRACKING, m_AnimateTime = 0.f, m_ActiveDamaged = false;

			m_AnimateTime += ElapsedTime;
		}
		break;

		case DEATH:
		{
			if (GetCurrentAnimationTrackIndex() != M_DEATH_A) SetAnimationTrack(M_DEATH_A, ANIMATION_TYPE_ONCE);

			switch (m_Type) {
			case M_WEAKORC:
			case M_STRONGORC:
			{
				// Weak & Strong : A (1.83334), B (1.6667)
				if (m_AnimateTime >= 1.799f) m_AnimateTime = 0.f, m_Death = true;
			}
			break;

			case M_SHAMANORC:
			{
				// Shaman : A (1.6667), B (1.6667)
				if (m_AnimateTime >= 1.599f) m_AnimateTime = 0.f, m_Death = true;
			}
			break;

			default:
			{
				// WolfRider : A (2.000001) B (2.0)
				if (m_AnimateTime >= 1.899f) m_AnimateTime = 0.f, m_Death = true;
			}
			break;
			}
			m_AnimateTime += ElapsedTime;
		}
		break;
		}
	}
}

void Monster::Animate(float ElapsedTime, DirectX::XMFLOAT4X4 TargetTransformPos, Terrain* GetTerrain, Effect* Signal)
{
	// 1. ��ɿ� ���� ������ �ൿ ����
	MoveToPlayer(ElapsedTime, TargetTransformPos, Signal);

	// ���Ͱ� Ȱ�� �����϶��� Animate && Render
	if (m_ActiveMonster) {
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
	if (m_ActiveMonster) GameObject::Render(CommandList);
}