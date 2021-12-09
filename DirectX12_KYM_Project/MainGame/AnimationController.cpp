#include "stdafx.h"
#include "AnimationController.h"

#include "GameObject.h"


AnimationSet::AnimationSet()
{

}

AnimationSet::~AnimationSet()
{
	if (m_KeyFrameTransformTime != nullptr) delete[] m_KeyFrameTransformTime;
	for (int i = 0; i < m_KeyFrameTransformCount; ++i) if (m_KeyFrameTransformPos[i] != nullptr) delete[] m_KeyFrameTransformPos[i];
	if (m_KeyFrameTransformPos != nullptr) delete[] m_KeyFrameTransformPos;
}

void AnimationSet::CreateKeyFrameTransform()
{
	m_KeyFrameTransformTime = new float[m_KeyFrameTransformCount];
	m_KeyFrameTransformPos = new DirectX::XMFLOAT4X4*[m_KeyFrameTransformCount];
}

void AnimationSet::InitAnimationSet()
{
	m_Type = ANIMATION_TYPE_LOOP;
	m_StartPositionTime = 0.f;
	m_ChangeAnimationTrack = false;
}

float AnimationSet::ResetPositionTime(float PositionTime)
{
	float ResetTime = 0.f;

	switch (m_Type) {
	case ANIMATION_TYPE_LOOP:
	{
		ResetTime = PositionTime - int(PositionTime / m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1]) * m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1];
	}
	break;

	case ANIMATION_TYPE_ONCE:
	{
		// 애니메이션을 처음부터 시작하기 위해 PositionTime을 저장
		if (m_StartPositionTime == 0.f) m_StartPositionTime = PositionTime;

		ResetTime = (PositionTime - m_StartPositionTime) - int((PositionTime - m_StartPositionTime) / m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1]) * m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1];

		// 애니메이션이 종료되었으므로 끝을 알림
		if ((PositionTime - m_StartPositionTime) / m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1] >= 1.f) {
			m_ChangeAnimationTrack = true;
		}
	}
	break;
	}

	return ResetTime;
}

DirectX::XMFLOAT4X4 AnimationSet::GetSRT(int FrameIndex, float PositionTime)
{
	DirectX::XMFLOAT4X4 TransformPos{};
	DirectX::XMStoreFloat4x4(&TransformPos, DirectX::XMMatrixIdentity());

	for (int i = 0; i < m_KeyFrameTransformCount - 1; ++i) {
		// Position Time이 i번째 KeyFrame Animation에 저장된 Time보다 크고 i+1번째보다 작다면 변환
		if ((m_KeyFrameTransformTime[i] <= PositionTime) && (m_KeyFrameTransformTime[i + 1] >= PositionTime)) {
			float t = (PositionTime - m_KeyFrameTransformTime[i]) / (m_KeyFrameTransformTime[i + 1] - m_KeyFrameTransformTime[i]);

			DirectX::XMVECTOR S0{}, R0{}, T0{}, S1{}, R1{}, T1{};
			DirectX::XMMatrixDecompose(&S0, &R0, &T0, DirectX::XMLoadFloat4x4(&m_KeyFrameTransformPos[i][FrameIndex]));
			DirectX::XMMatrixDecompose(&S1, &R1, &T1, DirectX::XMLoadFloat4x4(&m_KeyFrameTransformPos[i + 1][FrameIndex]));

			DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, t);
			DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, t);
			DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, t);
			DirectX::XMStoreFloat4x4(&TransformPos, DirectX::XMMatrixAffineTransformation(S, DirectX::XMVectorZero(), R, T));
			break;
		}
	}
	return TransformPos;
}

// --------------------
AnimationTrack::AnimationTrack()
{

}

AnimationTrack::~AnimationTrack()
{

}

// --------------------
AnimationController::AnimationController()
{

}

AnimationController::~AnimationController()
{
	if (m_AnimationSet != nullptr) delete[] m_AnimationSet;
	if (m_AnimationTrack != nullptr) delete[] m_AnimationTrack;

	if (m_BoneFrame != nullptr) delete[] m_BoneFrame;
}

void AnimationController::CreateAnimation(int AnimationCount)
{
	m_AnimationCount = AnimationCount;

	m_AnimationSet = new AnimationSet[m_AnimationCount];
	m_AnimationTrack = new AnimationTrack[m_AnimationCount];
}

void AnimationController::CreateBoneFrame(int BoneFrameCount)
{
	m_BoneFrameCount = BoneFrameCount;

	m_BoneFrame = new GameObject*[m_BoneFrameCount];
}

void AnimationController::AssignAniSetToAniTrack()
{
	// m_AnimationSet에 저장된 애니메이션 정보를 사용하기 위해 가져옴 (ex. IDLE, RUN 등의 TransformPos)
	if (m_AnimationSet != nullptr) {
		for (int i = 0; i < m_AnimationCount; ++i) {
			m_AnimationTrack[i].SetAnimationSet(&m_AnimationSet[i]);
		}
	}
	// 0번 애니메이션(IDLE)을 기본 상태로 설정
	SetAnimationTrack(P_IDLE, ANIMATION_TYPE_LOOP);
}

void AnimationController::SetAnimationTrack(int Index, int Type)
{
	if (m_AnimationTrack != nullptr) {
		// 기존에 하던 애니메이션 초기화
		for (int i = 0; i < m_AnimationCount; ++i) {
			m_AnimationTrack[i].SetActive(false);
			m_AnimationTrack[i].GetAnimationSet()->InitAnimationSet();
		}
		// 재설정
		m_AnimationTrack[Index].SetActive(true);
		m_AnimationTrack[Index].GetAnimationSet()->SetType(Type);
	}
	m_CurrentAnimationTrackIndex = Index;
}

void AnimationController::UpdateAnimationPos(float ElapsedTime)
{
	// 1. 이전 프레임에서 애니메이션 트랙을 바꾸라는 명령이 있었는지에 따라 수행
	if (m_NextAnimationTrackIndex != -1) {
		m_CurrentAnimationTrackIndex = m_NextAnimationTrackIndex;
		m_NextAnimationTrackIndex = -1;
		SetAnimationTrack(m_CurrentAnimationTrackIndex, ANIMATION_TYPE_LOOP);
	}
	// 2. 현재 활성화된 애니메이션 트랙의 애니메이션 수행
	if (m_AnimationTrack != nullptr) {
		if (m_AnimationTrack[m_CurrentAnimationTrackIndex].GetActive() == true) {
			AnimationSet* UsingAniSet = m_AnimationTrack[m_CurrentAnimationTrackIndex].GetAnimationSet();
			m_CumulativeTime += ElapsedTime;

			float PositionTime = UsingAniSet->ResetPositionTime(m_CumulativeTime);

			for (int i = 0; i < m_BoneFrameCount; ++i) {
				m_BoneFrame[i]->SetTransformPos(UsingAniSet->GetSRT(i, PositionTime));
			}
			// 3. 애니메이션 트랙의 변경 명령이 있으면 다음 프레임에 바꾸기 위해 값을 설정
			if (UsingAniSet->GetChangeAnimationTrack() == true) {
				m_NextAnimationTrackIndex = P_IDLE;
				UsingAniSet->InitAnimationSet();
			}
		}
	}
}