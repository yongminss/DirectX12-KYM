#include "stdafx.h"
#include "GameObject.h"

#include "Mesh.h"
#include "Material.h"
#include "AnimationController.h"


int ReadIntegerFile(FILE* File)
{
	int Value = 0;
	fread(&Value, sizeof(int), 1, File);

	return Value;
}

void ReadStringFile(FILE* File, char* Word)
{
	BYTE WordCount = '\0';

	fread(&WordCount, sizeof(BYTE), 1, File);
	fread(Word, sizeof(char), WordCount, File);
	Word[WordCount] = '\0';
}


GameObject::GameObject()
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());
}

GameObject::GameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	// 정육면체로 된 오브젝트를 생성
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	// 오브젝트의 정점들의 집합인 Mesh 생성
	Mesh *UsingMesh = new Mesh(Device, CommandList, 10.f);
	SetMesh(UsingMesh);

	// 오브젝트가 사용할 그래픽스 파이프라인을 생성
	Material *UsingMaterial = new Material();
	UsingMaterial->CreateMaterial(Device, CommandList, RootSignature, -1);
	SetMaterial(UsingMaterial);
}

GameObject::~GameObject()
{
	if (m_Mesh != nullptr) delete m_Mesh;
	if (m_Material != nullptr) delete m_Material;
	if (m_AnimationController != nullptr) delete m_AnimationController;
}

GameObject* GameObject::LoadBinaryFileModel(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, const char* FileName, Shader* InstanceShader, bool ActiveAnimation)
{
	FILE *File = nullptr;
	fopen_s(&File, FileName, "rb");
	rewind(File);

	GameObject *Model = LoadFrameHierarchy(Device, CommandList, RootSignature, File, nullptr, InstanceShader);

	Model->SetMeshBoneFrame(Model);

	if (ActiveAnimation == true) {
		Model->m_AnimationController = new AnimationController();
		Model->LoadAnimationInfo(File);
		Model->m_AnimationController->AssignAniSetToAniTrack();
	}
	fclose(File);

	return Model;
}

GameObject* GameObject::LoadFrameHierarchy(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature, FILE* File, GameObject* ParentFrame, Shader* InstanceShader)
{
	GameObject *Frame = nullptr;

	char Word[64] = { '\0' };
	int FrameCount = 0, TextureCount = 0;

	while (true) {
		ReadStringFile(File, Word);

		// 읽어온 단어(ex. Frame, Transform, Mesh ... etc)로 오브젝트에 대한 값을 설정
		if (!strcmp(Word, "<Frame>:")) {
			Frame = new GameObject();

			fread(&FrameCount, sizeof(int), 1, File);
			fread(&TextureCount, sizeof(int), 1, File);

			ReadStringFile(File, Frame->m_FrameName);
		}

		else if (!strcmp(Word, "<Transform>:")) {
			DirectX::XMFLOAT3 Position{}, Rotation{}, Scale{};
			DirectX::XMFLOAT4 Quaternion{};

			fread(&Position, sizeof(float), 3, File);
			fread(&Rotation, sizeof(float), 3, File);
			fread(&Scale, sizeof(float), 3, File);
			fread(&Quaternion, sizeof(float), 4, File);
		}

		else if (!strcmp(Word, "<TransformMatrix>:")) {
			fread(&Frame->m_TransformPos, sizeof(float), 16, File);
		}

		else if (!strcmp(Word, "<Children>:")) {
			int ChildCount = ReadIntegerFile(File);

			if (ChildCount > 0) {
				for (int i = 0; i < ChildCount; ++i) {
					GameObject *Child = LoadFrameHierarchy(Device, CommandList, RootSignature, File, Frame, InstanceShader);
					if (Child != nullptr) Frame->SetChild(Child);
				}
			}
		}

		else if (!strcmp(Word, "<Mesh>:")) {
			LoadedMesh *UsingMesh = new LoadedMesh();
			UsingMesh->LoadMeshInfo(Device, CommandList, File);
			Frame->SetMesh(UsingMesh);
		}

		else if (!strcmp(Word, "<Materials>:")) {
			Material *UsingMaterial = new Material();
			UsingMaterial->LoadMaterialInfo(Device, CommandList, RootSignature, File, ParentFrame, Frame->m_ShaderType, InstanceShader);
			Frame->SetMaterial(UsingMaterial);
		}

		else if (!strcmp(Word, "<SkinningInfo>:")) {
			Frame->m_ShaderType = 1;
			SkinnedMesh *UsingMesh = new SkinnedMesh();
			UsingMesh->CreateShaderBuffer(Device, CommandList);

			UsingMesh->LoadSkinInfo(Device, CommandList, File);

			ReadStringFile(File, Word);
			UsingMesh->LoadMeshInfo(Device, CommandList, File);

			Frame->SetMesh(UsingMesh);
		}

		else if (!strcmp(Word, "</Frame>")) {
			break;
		}
	}
	return Frame;
}

void GameObject::LoadAnimationInfo(FILE *File)
{
	char Word[64] = { '\0' };

	while (true) {
		ReadStringFile(File, Word);

		if (!strcmp(Word, "<AnimationSets>:")) {
			m_AnimationController->CreateAnimation(ReadIntegerFile(File));
		}

		else if (!strcmp(Word, "<FrameNames>:")) {
			m_AnimationController->CreateBoneFrame(ReadIntegerFile(File));

			for (int i = 0; i < m_AnimationController->GetBoneFrameCount(); ++i) {
				ReadStringFile(File, Word);
				m_AnimationController->SetBoneFrame(i, FindFrame(Word));
			}
		}

		else if (!strcmp(Word, "<AnimationSet>:")) {
			int AnimationIndex = ReadIntegerFile(File);

			AnimationSet *UsingAnimationSet = m_AnimationController->GetAnimationSet(AnimationIndex);

			ReadStringFile(File, Word);
			UsingAnimationSet->SetAnimationName(Word);

			float Length = 0.f;
			fread(&Length, sizeof(float), 1, File);
			UsingAnimationSet->SetLength(Length);

			int FramePerSecondCount = ReadIntegerFile(File);
			UsingAnimationSet->SetFramePerSecondCount(FramePerSecondCount);

			int KeyFrameTransformCount = ReadIntegerFile(File);
			UsingAnimationSet->SetKeyFrameTransformCount(KeyFrameTransformCount);

			// KeyFrame Transform Time & KeyFrame Transform 동적 할당
			UsingAnimationSet->CreateKeyFrameTransform();

			for (int i = 0; i < UsingAnimationSet->GetKeyFrameTransformCount(); ++i) {
				ReadStringFile(File, Word);

				if (!strcmp(Word, "<Transforms>:")) {
					int KeyFrameIndex = ReadIntegerFile(File);

					float KeyFrameTransformTime = 0.f;
					fread(&KeyFrameTransformTime, sizeof(float), 1, File);
					UsingAnimationSet->SetKeyFrameTransformTime(i, KeyFrameTransformTime);

					DirectX::XMFLOAT4X4 *KeyFrameTransform = new DirectX::XMFLOAT4X4[m_AnimationController->GetBoneFrameCount()];
					fread(KeyFrameTransform, sizeof(DirectX::XMFLOAT4X4), m_AnimationController->GetBoneFrameCount(), File);
					UsingAnimationSet->SetKeyFrameTransformPos(i, KeyFrameTransform);
				}
			}
		}

		else if (!strcmp(Word, "</AnimationSets>")) {
			break;
		}
	}
}

void GameObject::SetRight(DirectX::XMFLOAT3 Right)
{
	m_TransformPos._11 = Right.x;
	m_TransformPos._12 = Right.y;
	m_TransformPos._13 = Right.z;

	UpdateTransform(nullptr);
}

void GameObject::SetUp(DirectX::XMFLOAT3 Up)
{
	m_TransformPos._21 = Up.x;
	m_TransformPos._22 = Up.y;
	m_TransformPos._23 = Up.z;

	UpdateTransform(nullptr);
}

void GameObject::SetLook(DirectX::XMFLOAT3 Look)
{
	m_TransformPos._31 = Look.x;
	m_TransformPos._32 = Look.y;
	m_TransformPos._33 = Look.z;

	UpdateTransform(nullptr);
}

void GameObject::SetPosition(DirectX::XMFLOAT3 Position)
{
	m_TransformPos._41 = Position.x;
	m_TransformPos._42 = Position.y;
	m_TransformPos._43 = Position.z;

	UpdateTransform(nullptr);
}

void GameObject::SetTransformPos(DirectX::XMFLOAT4X4 TransformPos)
{
	m_TransformPos._11 = TransformPos._11, m_TransformPos._12 = TransformPos._12, m_TransformPos._13 = TransformPos._13, m_TransformPos._14 = TransformPos._14;
	m_TransformPos._21 = TransformPos._21, m_TransformPos._22 = TransformPos._22, m_TransformPos._23 = TransformPos._23, m_TransformPos._24 = TransformPos._24;
	m_TransformPos._31 = TransformPos._31, m_TransformPos._32 = TransformPos._32, m_TransformPos._33 = TransformPos._33, m_TransformPos._34 = TransformPos._34;
	m_TransformPos._41 = TransformPos._41, m_TransformPos._42 = TransformPos._42, m_TransformPos._43 = TransformPos._43, m_TransformPos._44 = TransformPos._44;

	UpdateTransform(nullptr);
}

void GameObject::SetScale(DirectX::XMFLOAT3 Size)
{
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(Size.x, Size.y, Size.z);
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixMultiply(Scale, DirectX::XMLoadFloat4x4(&m_TransformPos)));

	UpdateTransform(nullptr);
}

void GameObject::SetRotate(DirectX::XMFLOAT3 Angle)
{
	DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Angle.x), DirectX::XMConvertToRadians(Angle.y), DirectX::XMConvertToRadians(Angle.z));
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixMultiply(Rotate, DirectX::XMLoadFloat4x4(&m_TransformPos)));

	UpdateTransform(nullptr);
}

void GameObject::SetChangeTexcoords(DirectX::XMFLOAT4 ChangeTexcoords)
{
	if (m_Material != nullptr) m_Material->SetChangeTexcoords(ChangeTexcoords);

	if (m_Sibling != nullptr) m_Sibling->SetChangeTexcoords(ChangeTexcoords);
	if (m_Child != nullptr) m_Child->SetChangeTexcoords(ChangeTexcoords);
}

void GameObject::SetChild(GameObject* Child)
{
	if (m_Child != nullptr) {
		Child->m_Sibling = m_Child->m_Sibling;
		m_Child->m_Sibling = Child;
	}
	else
		m_Child = Child;
}

void GameObject::SetMeshBoneFrame(GameObject* RootFrame)
{
	if (m_ShaderType == 1) {
		SkinnedMesh *UsingMesh = (SkinnedMesh*)m_Mesh;
		for (int i = 0; i < UsingMesh->GetBoneCount(); ++i)
			UsingMesh->SetBoneFrame(i, RootFrame->FindFrame(UsingMesh->GetBoneName(i)));
	}
	if (m_Sibling != nullptr) m_Sibling->SetMeshBoneFrame(RootFrame);
	if (m_Child != nullptr) m_Child->SetMeshBoneFrame(RootFrame);
}

void GameObject::SetAnimationTrack(int Index, int Type, bool Conversion)
{
	if (m_AnimationController != nullptr) m_AnimationController->SetAnimationTrack(Index, Type, Conversion);

	if (m_Sibling != nullptr) m_Sibling->SetAnimationTrack(Index, Type, Conversion);
	if (m_Child != nullptr) m_Child->SetAnimationTrack(Index, Type, Conversion);
}

void GameObject::SetChangeState(bool ChangeState)
{
	if (m_AnimationController != nullptr) m_AnimationController->SetChangeState(ChangeState);

	if (m_Sibling != nullptr) m_Sibling->SetChangeState(ChangeState);
	if (m_Child != nullptr) m_Child->SetChangeState(ChangeState);
}

float GameObject::GetCollisionMeshDistance()
{
	if (m_Mesh != nullptr) return m_Mesh->GetDistance();
	return 0.f;
}

int GameObject::GetCurrentAnimationTrackIndex()
{
	if (m_AnimationController != nullptr) return m_AnimationController->GetCurrentAnimationTrackIndex();

	if (m_Sibling != nullptr) return m_Sibling->GetCurrentAnimationTrackIndex();
	if (m_Child != nullptr) return m_Child->GetCurrentAnimationTrackIndex();
}

bool GameObject::GetChangeState()
{
	if (m_AnimationController != nullptr) return m_AnimationController->GetChangeState();

	if (m_Sibling != nullptr) return m_Sibling->GetChangeState();
	if (m_Child != nullptr) return m_Child->GetChangeState();
}

GameObject* GameObject::FindFrame(char* FrameName)
{
	GameObject* Frame = nullptr;

	if (!strcmp(m_FrameName, FrameName)) return this;

	if (m_Sibling != nullptr) if (Frame = m_Sibling->FindFrame(FrameName)) return Frame;
	if (m_Child != nullptr) if (Frame = m_Child->FindFrame(FrameName)) return Frame;

	return nullptr;
}

GameObject* GameObject::GetFrame(int Index)
{
	if (m_AnimationController != nullptr) return m_AnimationController->GetBoneFrame(Index);

	if (m_Sibling != nullptr) return m_Sibling->GetFrame(Index);
	if (m_Child != nullptr) return m_Child->GetFrame(Index);
}

Texture* GameObject::FindDuplicatedTexture(char* TextureName)
{
	// 찾아야 하는 TextureName과 현재 프레임이 가지고 있는 TextureName을 비교
	if (m_Material != nullptr) {
		if (m_Material->GetTexture() != nullptr) {
			// 찾고 있는 TextureName과 현재 프레임의 TextureName의 형식을 같도록 설정
			int i = 0;
			for (i = 1; TextureName[i]; ++i) TextureName[i - 1] = TextureName[i];
			TextureName[i - 1] = '\0';

			char CompareWord[64] = { '\0' };
			strcpy_s(CompareWord, 64, "Model/Texture/");
			strcat_s(CompareWord, TextureName);
			strcat_s(CompareWord, ".dds");

			// TextureName이 같으므로 현재 프레임이 가지고 있는 Texture를 반환
			if (!strcmp(CompareWord, m_Material->GetTextureName())) return m_Material->GetTexture();
		}
	}

	// TextureName이 같지 않다면 Sibling or Child에서 Texture를 찾음
	Texture* OtherTexture = nullptr;

	if (m_Sibling != nullptr) if (OtherTexture = m_Sibling->FindDuplicatedTexture(TextureName)) return OtherTexture;
	if (m_Child != nullptr) if (OtherTexture = m_Child->FindDuplicatedTexture(TextureName)) return OtherTexture;

	return nullptr;
}

GameObject* GameObject::CheckCollision(DirectX::XMFLOAT3 StartPos, DirectX::XMFLOAT3 EndPos)
{
	if (m_Mesh != nullptr) {
		DirectX::BoundingBox CheckingBoundingBox = m_Mesh->GetBoundingBox();
		CheckingBoundingBox.Transform(CheckingBoundingBox, DirectX::XMLoadFloat4x4(&m_WorldPos));

		float Distance = 0.f;

		if (CheckingBoundingBox.Intersects(DirectX::XMLoadFloat3(&StartPos), DirectX::XMLoadFloat3(&EndPos), Distance) == true) {
			m_Mesh->SetDistance(Distance);
			return this;
		}
	}

	if (m_Sibling != nullptr) return m_Sibling->CheckCollision(StartPos, EndPos);
	if (m_Child != nullptr) return m_Child->CheckCollision(StartPos, EndPos);

	return nullptr;
}

void GameObject::UpdateTransform(DirectX::XMFLOAT4X4* Parents)
{
	if (Parents != nullptr) {
		DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_TransformPos), DirectX::XMLoadFloat4x4(Parents)));
	}
	else {
		DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMLoadFloat4x4(&m_TransformPos));
	}

	if (m_Sibling != nullptr) m_Sibling->UpdateTransform(Parents);
	if (m_Child != nullptr) m_Child->UpdateTransform(&m_WorldPos);
}

void GameObject::Animate(float ElapsedTime)
{
	if (m_AnimationController != nullptr) m_AnimationController->UpdateAnimationPos(ElapsedTime);

	if (m_Sibling != nullptr) m_Sibling->Animate(ElapsedTime);
	if (m_Child != nullptr) m_Child->Animate(ElapsedTime);
}

void GameObject::UpdateShaderCode(ID3D12GraphicsCommandList* CommandList)
{
	DirectX::XMFLOAT4X4 WorldPos{};

	DirectX::XMStoreFloat4x4(&WorldPos, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_WorldPos)));
	CommandList->SetGraphicsRoot32BitConstants(1, 16, &WorldPos, 0);
}

void GameObject::Render(ID3D12GraphicsCommandList* CommandList)
{
	UpdateShaderCode(CommandList);

	if (m_Material != nullptr) m_Material->SetPipeline(CommandList);
	if (m_Material != nullptr) m_Material->MappingTexture(CommandList, 0);

	if (m_Mesh != nullptr) m_Mesh->Render(CommandList);

	if (m_Sibling != nullptr) m_Sibling->Render(CommandList);
	if (m_Child != nullptr) m_Child->Render(CommandList);
}

void GameObject::Render(ID3D12GraphicsCommandList* CommandList, D3D12_VERTEX_BUFFER_VIEW InstanceBufferView, int ModelCount)
{
	if (m_Material != nullptr) m_Material->SetPipeline(CommandList);
	if (m_Material != nullptr) m_Material->MappingTexture(CommandList, 0);

	if (m_Mesh != nullptr) m_Mesh->Render(CommandList, InstanceBufferView, ModelCount);

	if (m_Sibling != nullptr) m_Sibling->Render(CommandList, InstanceBufferView, ModelCount);
	if (m_Child != nullptr) m_Child->Render(CommandList, InstanceBufferView, ModelCount);
}