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
		// �ִϸ��̼��� ó������ �����ϱ� ���� PositionTime�� ����
		if (m_StartPositionTime == 0.f) m_StartPositionTime = PositionTime;

		ResetTime = (PositionTime - m_StartPositionTime) - int((PositionTime - m_StartPositionTime) / m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1]) * m_KeyFrameTransformTime[m_KeyFrameTransformCount - 1];

		// �ִϸ��̼��� ����Ǿ����Ƿ� ���� �˸�
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
		// Position Time�� i��° KeyFrame Animation�� ����� Time���� ũ�� i+1��°���� �۴ٸ� ��ȯ
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
	// m_AnimationSet�� ����� �ִϸ��̼� ������ ����ϱ� ���� ������ (ex. IDLE, RUN ���� TransformPos)
	if (m_AnimationSet != nullptr) {
		for (int i = 0; i < m_AnimationCount; ++i) {
			m_AnimationTrack[i].SetAnimationSet(&m_AnimationSet[i]);
		}
	}
	// 0�� �ִϸ��̼�(IDLE)�� �⺻ ���·� ����
	SetAnimationTrack(P_IDLE, ANIMATION_TYPE_LOOP);
}

void AnimationController::SetAnimationTrack(int Index, int Type)
{
	if (m_AnimationTrack != nullptr) {
		// ������ �ϴ� �ִϸ��̼� �ʱ�ȭ
		for (int i = 0; i < m_AnimationCount; ++i) {
			m_AnimationTrack[i].SetActive(false);
			m_AnimationTrack[i].GetAnimationSet()->InitAnimationSet();
		}
		// �缳��
		m_AnimationTrack[Index].SetActive(true);
		m_AnimationTrack[Index].GetAnimationSet()->SetType(Type);
	}
	m_CurrentAnimationTrackIndex = Index;
}

void AnimationController::UpdateAnimationPos(float ElapsedTime)
{
	// 1. ���� �����ӿ��� �ִϸ��̼� Ʈ���� �ٲٶ�� ����� �־������� ���� ����
	if (m_NextAnimationTrackIndex != -1) {
		m_CurrentAnimationTrackIndex = m_NextAnimationTrackIndex;
		m_NextAnimationTrackIndex = -1;
		SetAnimationTrack(m_CurrentAnimationTrackIndex, ANIMATION_TYPE_LOOP);
	}
	// 2. ���� Ȱ��ȭ�� �ִϸ��̼� Ʈ���� �ִϸ��̼� ����
	if (m_AnimationTrack != nullptr) {
		if (m_AnimationTrack[m_CurrentAnimationTrackIndex].GetActive() == true) {
			AnimationSet* UsingAniSet = m_AnimationTrack[m_CurrentAnimationTrackIndex].GetAnimationSet();
			m_CumulativeTime += ElapsedTime;

			float PositionTime = UsingAniSet->ResetPositionTime(m_CumulativeTime);

			for (int i = 0; i < m_BoneFrameCount; ++i) {
				m_BoneFrame[i]->SetTransformPos(UsingAniSet->GetSRT(i, PositionTime));
			}
			// 3. �ִϸ��̼� Ʈ���� ���� ����� ������ ���� �����ӿ� �ٲٱ� ���� ���� ����
			if (UsingAniSet->GetChangeAnimationTrack() == true) {
				m_NextAnimationTrackIndex = P_IDLE;
				UsingAniSet->InitAnimationSet();
			}
		}
	}
}