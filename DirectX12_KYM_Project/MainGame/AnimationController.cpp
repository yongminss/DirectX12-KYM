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

float AnimationSet::ResetPositionTime(float PositionTime)
{
	float ResetTime = 0.f;

	// Loop Animation
	ResetTime = PositionTime - int(PositionTime / m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1]) * m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1];

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
	ActiveAnimation(0);
}

int AnimationController::GetActiveTrackIndex()
{
	for (int i = 0; i < m_AnimationCount; ++i)
		if (m_AnimationTrack[i].GetActive() == true) return i;
}

void AnimationController::ActiveAnimation(int AnimationIndex)
{
	if (m_AnimationTrack != nullptr) {
		for (int i = 0; i < m_AnimationCount; ++i)
			m_AnimationTrack[i].SetActive(false);
		m_AnimationTrack[AnimationIndex].SetActive(true);
	}
}

void AnimationController::UpdateAnimationPos(float ElapsedTime)
{
	if (m_AnimationSet != nullptr) {
		for (int i = 0; i < m_AnimationCount; ++i) {
			if (m_AnimationTrack[i].GetActive() == true) {
				AnimationSet* UsingAniSet = m_AnimationTrack[i].GetAnimationSet();
				m_CumulativeTime += ElapsedTime;

				float PositionTime = UsingAniSet->ResetPositionTime(m_CumulativeTime);

				for (int i = 0; i < m_BoneFrameCount; ++i) {
					m_BoneFrame[i]->SetTransformPos(UsingAniSet->GetSRT(i, PositionTime));
				}
			}
		}
	}
}