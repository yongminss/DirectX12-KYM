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
	TargetPosition.y -= 25.f; // TargetPosition은 Root Frame이므로 플레이어 모델의 몸통을 기준으로 설정

	// 몬스터와 플레이어의 거리에 따라 몬스터의 Animate && Render를 설정
	DirectX::XMFLOAT3 CalDistance{};
	DirectX::XMStoreFloat3((&CalDistance), DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition()))));

	float Distance = CalDistance.x;

	if ((1000.f * 1000.f) >= (Distance * Distance) && false == m_Death) {
		m_ActiveMonster = true;
	}
	else m_ActiveMonster = false;

	if (true == m_ActiveMonster) {
		// 몬스터가 피격 당했을 때 피격 효과 (ex. 순간적인 색상 변화 && 피격 애니메이션)
		if (true == m_ActiveDamaged && m_State != 3) {
			if (0.f == m_HitTime) {
				m_State = 0;
				SetDamaged(0x02); // 피격된 것을 직관적으로 보여주기 위해 색상을 변경

				// 첫 피격일 경우 Signal을 이용하여 몬스터가 플레이어를 인식했다는 것을 보여줌
				if (false == m_FirstHit) {
					Signal->ActiveEffect(m_TransformPos), m_FirstHit = true;
				}

				// 몬스터의 피격 애니메이션 수행 - Shaman은 애니메이션의 인덱스가 다르므로 타입에 따른 결정
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

			// 일정 시간이 지나면 원래 색상으로 돌아옴
			if (m_HitTime >= 0.15f) {
				SetDamaged(0x00);
			}

			// 피격 애니메이션의 시간 0.6667초가 지나면 몬스터의 다음 행동을 결정
			if (m_HitTime >= 0.6f) {
				m_State = 1;
				m_ActiveDamaged = false, m_HitTime = 0.f;
			}
		}
		// 피격 외의 몬스터 행동을 결정
		else {
			if (0.f >= m_Hp) m_State = 3;

			// State 값에 따라 몬스터의 행동 설정 - 0 (플레이어 인식), 1 (플레이어 추격), 2 (플레이어 공격) 3 (몬스터 사망)
			switch (m_State) {
			case 0: // Ready State
			{
				// 1. 플레이어의 월드 좌표에서 몬스터의 월드 좌표를 뺄셈하여 몬스터의 방향 벡터를 계산
				DirectX::XMFLOAT3 NewLook{};
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
				// 정규화
				DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

				// 2. 새로운 Look 벡터를 계산했으므로 Right 벡터도 변경
				DirectX::XMFLOAT3 NewRight{};
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
				// 정규화
				DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

				// 3. 계산된 Right, Look 벡터를 대입
				SetRight(NewRight), SetLook(NewLook);

				// 플레이어와 몬스터 간의 거리가 너무 가까워지면 몬스터를 뒤로 이동
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
				// 공격 범위보다 플레이어의 거리가 멀 경우, 플레이어를 추격
				if ((60.f * 60.f) <= (Distance * Distance)) {
					SetAnimationTrack(M_RUN, ANIMATION_TYPE_LOOP);

					DirectX::XMFLOAT3 NewLook{};
					DirectX::XMStoreFloat3(&NewLook, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&TargetPosition), DirectX::XMLoadFloat3(&GetPosition())));
					DirectX::XMStoreFloat3(&NewLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewLook)));

					DirectX::XMFLOAT3 NewRight{};
					DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&GetUp()), DirectX::XMLoadFloat3(&NewLook)));
					DirectX::XMStoreFloat3(&NewRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&NewRight)));

					SetRight(NewRight), SetLook(NewLook);

					// 몬스터가 플레이어가 있는 방향으로 이동
					DirectX::XMFLOAT3 NewPosition{};
					DirectX::XMStoreFloat3(&NewPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&GetLook()), ElapsedTime * 100.f)));

					SetPosition(NewPosition);
				}
				// 거리가 가까워지면 플레이어를 공격
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

				// 타입 별로 애니메이션의 시간이 다르므로 분류
				switch (m_Type) {
				case 0:
				case 1:
				{
					// 무기를 휘둘렀을 때, 플레이어가 범위 내에 있으면 공격 성공처리
					if (false == m_SuccessAttack && m_AnimateTime > 0.3f && (60.f * 60.f) > (Distance * Distance)) m_SuccessAttack = true;

					// Weak & Strong : A (1.0), B (1.0)
					if (m_AnimateTime >= 0.999f) m_AnimateTime = 0.f, m_State = 1;
				}
				break;

				default:
				{
					// Shaman : 1.33, WolfRider : A (1.3334), B (1.3334)
					if (m_AnimateTime >= 1.329f) m_AnimateTime = 0.f, m_State = 1;

					// 무기를 휘둘렀을 때, 플레이어가 범위 내에 있으면 공격 성공처리
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
	// 1. 명령에 따라 몬스터의 행동 결정
	MoveToPlayer(ElapsedTime, TargetTransformPos, Signal);

	// 몬스터가 활성 상태일때만 Animate && Render
	if (true == m_ActiveMonster) {
		// 2. Scene에 있는 HeightMap의 높이에 따라 몬스터의 y좌표를 설정
		int GetHeightMapX = int(m_TransformPos._41) / MAP_SCALE, GetHeightMapZ = int(m_TransformPos._43) / MAP_SCALE;
		float GetHeightMapY = GetTerrain->GetHeightMapYPos(GetHeightMapX, GetHeightMapZ);

		SetPosition(DirectX::XMFLOAT3(m_TransformPos._41, GetHeightMapY, m_TransformPos._43));

		// 3. 위에서 결정된 몬스터의 애니메이션을 수행
		GameObject::Animate(ElapsedTime);
	}
}

void Monster::Render(ID3D12GraphicsCommandList* CommandList)
{
	if (true == m_ActiveMonster) GameObject::Render(CommandList);
}