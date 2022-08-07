#pragma once

class GameObject;

// 애니메이션의 이름, 걸리는 시간, 변환 값 등의 정보를 가지고 있음
class AnimationSet
{
private:
	char m_AnimationName[64]{};
	float m_Length = 0.f;
	int m_FramePerSecondCount = 0;
	int m_KeyFrameTransformCount = 0;
	float *m_KeyFrameTransformTime = nullptr;
	DirectX::XMFLOAT4X4 **m_KeyFrameTransformPos = nullptr;

	int m_Type = ANIMATION_TYPE_LOOP;
	float m_StartPositionTime = 0.f;
	bool m_ChangeAnimationTrack = false;

public:
	AnimationSet();
	~AnimationSet();

	void CreateKeyFrameTransform();

	void SetAnimationName(char* AnimationName) { memcpy(m_AnimationName, AnimationName, sizeof(m_AnimationName)); }
	void SetLength(float Length) { m_Length = Length; }
	void SetFramePerSecondCount(int FramePerSecondCount) { m_FramePerSecondCount = FramePerSecondCount; }
	void SetKeyFrameTransformCount(int KeyFrameTransformCount) { m_KeyFrameTransformCount = KeyFrameTransformCount; }
	void SetKeyFrameTransformTime(int Index, float KeyFrameTransformTime) { m_KeyFrameTransformTime[Index] = KeyFrameTransformTime; }
	void SetKeyFrameTransformPos(int Index, DirectX::XMFLOAT4X4* KeyFrameTransformPos) { m_KeyFrameTransformPos[Index] = KeyFrameTransformPos; }
	void SetType(int Type) { m_Type = Type; }

	void SetStartPosTime(float StartPositionTime) { m_StartPositionTime = StartPositionTime; }

	int GetKeyFrameTransformCount() { return m_KeyFrameTransformCount; }
	float GetStartPosTime() { return m_StartPositionTime; }
	bool GetChangeAnimationTrack() { return m_ChangeAnimationTrack; }

	void InitAnimationSet();

	float ResetPositionTime(float PositionTime);
	DirectX::XMFLOAT4X4 GetSRT(int FrameIndex, float PositionTime);
};

// AnimationSet을 저장 (ex. 1번 AnimationTrack은 어떤 오브젝트의 1번 애니메이션(ex. walk)을 가짐
class AnimationTrack
{
private:
	bool m_Active = false;

	AnimationSet *m_AnimationSet = nullptr;

public:
	AnimationTrack();
	~AnimationTrack();

	void SetActive(bool Active) { m_Active = Active; }
	void SetAnimationSet(AnimationSet *UsingAnimationSet) { m_AnimationSet = UsingAnimationSet; }

	bool GetActive() { return m_Active; }
	AnimationSet* GetAnimationSet() { return m_AnimationSet; }
};

// 애니메이션을 직접적으로 관리 (ex. 경과 시간에 맞추어 애니메이션 변환 or 타입에 맞추어 애니메이션 반복 등)
class AnimationController
{
private:
	int m_AnimationCount = 0;
	AnimationSet *m_AnimationSet = nullptr;
	AnimationTrack *m_AnimationTrack = nullptr;

	int m_BoneFrameCount = 0;
	GameObject **m_BoneFrame = nullptr;

	float m_CumulativeTime = 0.f;

	int m_CurrentAnimationTrackIndex = 0;
	int m_NextAnimationTrackIndex = 0;

	bool m_ChangeState = false;

public:
	AnimationController();
	~AnimationController();

	void CreateAnimation(int AnimationCount);
	void CreateBoneFrame(int BoneFrameCount);
	void AssignAniSetToAniTrack();

	void SetBoneFrame(int Index, GameObject *Frame) { m_BoneFrame[Index] = Frame; }
	void SetChangeState(bool ChangeState) { m_ChangeState = ChangeState; }

	void SetAnimationTrack(int Index, int Type, bool Conversion = false);

	GameObject* GetBoneFrame(int Index) { return m_BoneFrame[Index]; }
	int GetAnimationCount() { return m_AnimationCount; }
	AnimationSet* GetAnimationSet(int Index) { return &m_AnimationSet[Index]; }
	int GetBoneFrameCount() { return m_BoneFrameCount; }
	int GetCurrentAnimationTrackIndex() { return m_CurrentAnimationTrackIndex; }
	bool GetChangeState() { return m_ChangeState; }

	void UpdateAnimationPos(float ElapsedTime);
};