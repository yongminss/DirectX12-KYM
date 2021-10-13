#pragma once

class GameObject;

// -
class AnimationSet
{
private:
	char m_AnimationName[64]{};
	float m_Length = 0.f;
	int m_FramePerSecondCount = 0;
	int m_KeyFrameTransformCount = 0;
	float *m_KeyFrameTransformTime = nullptr;
	DirectX::XMFLOAT4X4 **m_KeyFrameTransformPos = nullptr;

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

	int GetKeyFrameTransformCount() { return m_KeyFrameTransformCount; }

	float ResetPositionTime(float PositionTime);
	DirectX::XMFLOAT4X4 GetSRT(int FrameIndex, float PositionTime);
};

// -
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

// -
class AnimationController
{
private:
	int m_AnimationCount = 0;
	AnimationSet *m_AnimationSet = nullptr;
	AnimationTrack *m_AnimationTrack = nullptr;

	int m_BoneFrameCount = 0;
	GameObject **m_BoneFrame = nullptr;

	float m_CumulativeTime = 0.f;

public:
	AnimationController();
	~AnimationController();

	void CreateAnimation(int AnimationCount);
	void CreateBoneFrame(int BoneFrameCount);
	void AssignAniSetToAniTrack();

	void SetBoneFrame(int Index, GameObject *Frame) { m_BoneFrame[Index] = Frame; }

	int GetAnimationCount() { return m_AnimationCount; }
	AnimationSet* GetAnimationSet(int Index) { return &m_AnimationSet[Index]; }
	int GetBoneFrameCount() { return m_BoneFrameCount; }

	void ActiveAnimation(int AnimationIndex);

	void UpdateAnimationPos(float ElapsedTime);
};