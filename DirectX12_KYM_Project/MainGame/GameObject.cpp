#include "stdafx.h"
#include "GameObject.h"


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

	std::cout << "Word : " << Word << std::endl;
}

void testFunction(FILE *File)
{
	char strTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, File);
	nRead = (UINT)::fread(strTextureName, sizeof(char), nStrLength, File);
	strTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	if (strcmp(strTextureName, "null")) {

		//SetType(nType);

		char FilePath[64] = { '\0' };
		strcpy_s(FilePath, 64, "Model/Textures/");

		bDuplicated = (strTextureName[0] == '@');
		strcpy_s(FilePath + 15, 64 - 15, (bDuplicated) ? (strTextureName + 1) : strTextureName);
		strcpy_s(FilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		_TCHAR *TextureName[64] = { '\0' };
		size_t nConverted = 0;
		//mbstowcs_s(&nConverted, TextureName, 64, FilePath, _TRUNCATE);

		/*if (!bDuplicated) {
			*ObjTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
			(*ObjTexture)->LoadTextureFromFile(Device, CommandList, TextureName, 0);
			if (*ObjTexture) (*ObjTexture)->AddRef();

			GameScene::CreateShaderResourceView(Device, CommandList, *ObjTexture, nRootParameter, false);
		}
		else {
			if (Parent) {
				while (Parent) {
					if (!Parent->m_Parent) break;
					Parent = Parent->m_Parent;
				}
				if (*ObjTexture) (*ObjTexture)->AddRef();
				GameObject *RootGameObj = Parent;
				*ObjTexture = RootGameObj->FindReplicatedTexture(TextureName);
			}
		}*/
	}
}


GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	for (int i = 0; i < m_MeshCount; ++i) if (m_Mesh[i] != nullptr) delete m_Mesh[i];
	delete[] m_Mesh;
	if (m_Shader != nullptr) delete m_Shader;
	if (m_Texture != nullptr) delete m_Texture;
}

void GameObject::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransformPos, DirectX::XMMatrixIdentity());

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	// 오브젝트의 정점들의 집합인 Mesh 생성
	Mesh *UsingMesh = new Mesh();
	UsingMesh->CreateMesh(Device, CommandList, 10.f);
	SetMesh(0, UsingMesh);

	// 오브젝트가 사용할 그래픽스 파이프라인을 생성
	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);
}

void GameObject::LoadModel()
{
	/*const char FileName[64] = "Monster.bin";

	FILE *File = nullptr;
	fopen_s(&File, FileName, "rb");
	rewind(File);

	GameObject* UsingObj = LoadModel2(File);*/


	FILE *File = NULL;
	::fopen_s(&File, "Monster.bin", "rb");
	::rewind(File);

	GameObject *GameObj = GameObject::LoadModel2(File);
}

GameObject* GameObject::LoadModel2(FILE* File)
{
	//GameObject* Model = nullptr;
	//char Word[64] = { '\0' };

	//int FrameCount = 0, TextureCount = 0;

	//while (true) {
	//	// 파일 읽음
	//	ReadStringFile(File, Word);

	//	// 파일에서 읽어온 단어에 따라 분기
	//	if (!strcmp(Word, "<Frame>:")) {

	//		Model = new GameObject();

	//		fread(&FrameCount, sizeof(int), 1, File);
	//		fread(&TextureCount, sizeof(int), 1, File);

	//		std::cout << "Frame Count : " << FrameCount << std::endl;
	//		std::cout << "Texture Count : " << TextureCount << std::endl;

	//		char FrameName[64] = { '\0' };
	//		ReadStringFile(File, FrameName);

	//		Model->SetFrameName(FrameName);

	//		std::cout << "Frame Name of Model : " << Model->GetFrameName() << std::endl;
	//	}

	//	else if (!strcmp(Word, "<Transform>:")) {

	//		DirectX::XMFLOAT3 Position{}, Rotation{}, Scale{};
	//		DirectX::XMFLOAT4 Quaternion{};

	//		fread(&Position, sizeof(float), 3, File);
	//		fread(&Rotation, sizeof(float), 3, File);
	//		fread(&Scale, sizeof(float), 3, File);
	//		fread(&Quaternion, sizeof(float), 4, File);

	//		std::cout << "Position : " << Position.x << ", " << Position.y << ", " << Position.z << std::endl;
	//		std::cout << "Rotation : " << Rotation.x << ", " << Rotation.y << ", " << Rotation.z << std::endl;
	//		std::cout << "Scale : " << Scale.x << ", " << Scale.y << ", " << Scale.z << std::endl;
	//		std::cout << "Quaternion : " << Quaternion.x << ", " << Quaternion.y << ", " << Quaternion.z << ", " << Quaternion.w << std::endl;
	//	}

	//	else if (!strcmp(Word, "<TransformMatrix>:")) {
	//		DirectX::XMFLOAT4X4 TransformPos{};
	//		fread(&TransformPos, sizeof(float), 16, File);

	//		Model->SetTransformPos(TransformPos);

	//		std::cout << Model->m_TransformPos._11 << ", " << Model->m_TransformPos._12 << ", " << Model->m_TransformPos._13 << ", " << Model->m_TransformPos._14 << std::endl;
	//		std::cout << Model->m_TransformPos._21 << ", " << Model->m_TransformPos._22 << ", " << Model->m_TransformPos._23 << ", " << Model->m_TransformPos._24 << std::endl;
	//		std::cout << Model->m_TransformPos._31 << ", " << Model->m_TransformPos._32 << ", " << Model->m_TransformPos._33 << ", " << Model->m_TransformPos._34 << std::endl;
	//		std::cout << Model->m_TransformPos._41 << ", " << Model->m_TransformPos._42 << ", " << Model->m_TransformPos._43 << ", " << Model->m_TransformPos._44 << std::endl;
	//	}

	//	else if (!strcmp(Word, "<Children>:")) {
	//		int ChildCount = ReadIntegerFile(File);
	//		std::cout << "Child Count : " << ChildCount << std::endl;
	//		if (ChildCount > 0) {
	//			GameObject *Child = GameObject::LoadModel2(File);
	//		}
	//	}

	//	else if (!strcmp(Word, "<Mesh>:")) {
	//		LoadFileMesh *UsingMesh = new LoadFileMesh();
	//		UsingMesh->LoadFile(File);
	//		//SetMesh(0, UsingMesh);
	//	}

	//	else if (!strcmp(Word, "<Materials>:")) {
	//		int MaterialCount = ReadIntegerFile(File);
	//		std::cout << "Material Count : " << MaterialCount << std::endl;

	//		while (true) {
	//			ReadStringFile(File, Word);

	//			if (!strcmp(Word, "<Material>:")) {
	//				int MaterialIndex = ReadIntegerFile(File);

	//				// Set Material
	//			}

	//			else if (!strcmp(Word, "<AlbedoColor>:")) {
	//				DirectX::XMFLOAT4 Albedo{};
	//				fread(&Albedo, sizeof(float), 4, File);

	//				std::cout << "Albedo Color : " << Albedo.x << ", " << Albedo.y << ", " << Albedo.z << ", " << Albedo.w << std::endl;
	//			}

	//			else if (!strcmp(Word, "<EmissiveColor>:")) {
	//				DirectX::XMFLOAT4 Emissive{};
	//				fread(&Emissive, sizeof(float), 4, File);

	//				std::cout << "Emissive Color : " << Emissive.x << ", " << Emissive.y << ", " << Emissive.z << ", " << Emissive.w << std::endl;
	//			}

	//			else if (!strcmp(Word, "<Glossiness>:")) {
	//				float Glossiness = 0.f;
	//				fread(&Glossiness, sizeof(float), 1, File);

	//				std::cout << "Glossiness : " << Glossiness << std::endl;
	//			}

	//			else if (!strcmp(Word, "<Metallic>:")) {
	//				float Metallic = 0.f;
	//				fread(&Metallic, sizeof(float), 1, File);

	//				std::cout << "Metallic : " << Metallic << std::endl;
	//			}

	//			else if (!strcmp(Word, "<SpecularHighlight>:")) {
	//				float Highlight = 0.f;
	//				fread(&Highlight, sizeof(float), 1, File);

	//				std::cout << "Specular Highlight : " << Highlight << std::endl;
	//			}

	//			else if (!strcmp(Word, "<GlossyReflection>:")) {
	//				float Reflection = 0.f;
	//				fread(&Reflection, sizeof(float), 1, File);

	//				std::cout << "Glossy Reflection : " << Reflection << std::endl;
	//			}

	//			else if (!strcmp(Word, "<AlbedoMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Albedo Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "<MetallicMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Metallic Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "<NormalMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Normal Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "<EmissionMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Emissive Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "<DetailAlbedoMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Detail Albedo Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "<DetailNormalMap>:")) {
	//				char TextureName[64] = { '\0' };
	//				ReadStringFile(File, TextureName);

	//				std::cout << "Detail Normal Texture Name : " << TextureName << std::endl;
	//			}

	//			else if (!strcmp(Word, "</Materials>")) {
	//				break;
	//			}
	//		}
	//	}

	//	else if (!strcmp(Word, "</Frame>")) {
	//		break;
	//	}
	//}

	//return Model;


	char Token[64] = { '\0' };
	int nFrame = 0, nTexture = 0;

	GameObject *GameObj = NULL;

	int m_nMaterial = 0;

	for (; ;) {

		ReadStringFile(File, Token);

		if (!strcmp(Token, "<Frame>:")) {

			GameObj = new GameObject();

			fread(&nFrame, sizeof(int), 1, File);
			fread(&nTexture, sizeof(int), 1, File);

			ReadStringFile(File, GameObj->m_FrameName);
		}

		else if (!strcmp(Token, "<Transform>:")) {

			DirectX::XMFLOAT3 Position, Rotation, Scale;
			DirectX::XMFLOAT4 Quaternion;
			fread(&Position, sizeof(float), 3, File);
			fread(&Rotation, sizeof(float), 3, File);
			fread(&Scale, sizeof(float), 3, File);
			fread(&Quaternion, sizeof(float), 4, File);
		}

		else if (!strcmp(Token, "<TransformMatrix>:"))
			fread(&GameObj->m_TransformPos, sizeof(float), 16, File);

		else if (!strcmp(Token, "<Mesh>:")) {

			LoadFileMesh *UsingMesh = new LoadFileMesh[5];
			UsingMesh->LoadFile(File);

			//SetMesh(0, UsingMesh);

			/*StandardMesh *ObjMesh = new StandardMesh(Device, CommandList);
			ObjMesh->LoadMeshFromFile(Device, CommandList, InFile);
			if (ObjMesh) GameObj->SetMesh(ObjMesh);*/
		}

		/*else if (!strcmp(Token, "<SkinningInfo>:")) {
			SkinnedMesh *ObjMesh = new SkinnedMesh(Device, CommandList);
			ObjMesh->CreateShaderVariable(Device, CommandList);

			ObjMesh->LoadSkinInfoFromFile(Device, CommandList, InFile);

			ReadStringFromFile(InFile, Token);
			ObjMesh->LoadMeshFromFile(Device, CommandList, InFile);

			GameObj->SetMesh(ObjMesh);
		}*/

		else if (!strcmp(Token, "<Materials>:")) {

			char Token[64] = { '\0' };
			UINT nRead = 0;
			int nMaterial = 0;

			m_nMaterial = ReadIntegerFile(File);

			/*m_Material = new Material*[m_nMaterial];
			for (int i = 0; i < m_nMaterial; ++i) m_Material[i] = NULL;

			Material *ObjMaterial = NULL;*/

			for (; ;) {
				ReadStringFile(File, Token);

				if (!strcmp(Token, "<Material>:")) {
					nMaterial = ReadIntegerFile(File);

					/*ObjMaterial = new Material(7);

					if (!Shader) {
						UINT nMeshType = GetMeshType();
						if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
							if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
								ObjMaterial->SetSkinnedAnimationShader();
							else
								ObjMaterial->SetStandardShader();
					}
					else
						SetShader(0, Shader);

					SetMaterial(nMaterial, ObjMaterial);*/
				}

				else if (!strcmp(Token, "<AlbedoColor>:")) {
					DirectX::XMFLOAT4 m_Albedo{};
					fread(&m_Albedo, sizeof(float), 4, File);
				}

				else if (!strcmp(Token, "<EmissiveColor>:")) {
					DirectX::XMFLOAT4 m_Emissive{};
					nRead = (UINT)::fread(&m_Emissive, sizeof(float), 4, File);
				}

				else if (!strcmp(Token, "<SpecularColor>:")) {
					DirectX::XMFLOAT4 m_Specular{};
					nRead = (UINT)::fread(&m_Specular, sizeof(float), 4, File);
				}

				else if (!strcmp(Token, "<Glossiness>:")) {
					float m_Glossiness = 0.f;
					nRead = (UINT)::fread(&m_Glossiness, sizeof(float), 1, File);
				}

				else if (!strcmp(Token, "<Smoothness>:")) {
					float m_Smoothness = 0.f;
					nRead = (UINT)::fread(&m_Smoothness, sizeof(float), 1, File);
				}

				else if (!strcmp(Token, "<Metallic>:")) {
					float m_Metallic = 0.f;
					nRead = (UINT)::fread(&m_Metallic, sizeof(float), 1, File);
				}

				else if (!strcmp(Token, "<SpecularHighlight>:")) {
					float m_SpecularHighlight = 0.f;
					nRead = (UINT)::fread(&m_SpecularHighlight, sizeof(float), 1, File);
				}

				else if (!strcmp(Token, "<GlossyReflection>:")) {
					float m_GlossyReflection = 0.f;
					nRead = (UINT)::fread(&m_GlossyReflection, sizeof(float), 1, File);
				}

				else if (!strcmp(Token, "<AlbedoMap>:")) {
					testFunction(File);
				}
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 3, ObjMaterial->m_TextureName[0], &(ObjMaterial->m_Texture[0]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<SpecularMap>:")) {
					testFunction(File);
				}
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_SPECULAR_MAP, 4, ObjMaterial->m_TextureName[1], &(ObjMaterial->m_Texture[1]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<NormalMap>:")) testFunction(File);
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 5, ObjMaterial->m_TextureName[2], &(ObjMaterial->m_Texture[2]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<MetallicMap>:")) testFunction(File);
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 6, ObjMaterial->m_TextureName[3], &(ObjMaterial->m_Texture[3]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<EmissionMap>:")) testFunction(File);
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 7, ObjMaterial->m_TextureName[4], &(ObjMaterial->m_Texture[4]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<DetailAlbedoMap>:")) testFunction(File);
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 8, ObjMaterial->m_TextureName[5], &(ObjMaterial->m_Texture[5]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "<DetailNormalMap>:")) testFunction(File);
				//m_Material[nMaterial]->LoadTexutreFromFile(Device, CommandList, MATERIAL_ALBEDO_MAP, 9, ObjMaterial->m_TextureName[6], &(ObjMaterial->m_Texture[6]), InFile, Parent, Shader, ObjMaterial);

				else if (!strcmp(Token, "</Materials>"))
					break;
			}
		}

		else if (!strcmp(Token, "<SkinningInfo>:")) {

			//char Token[64] = { '\0' };
			BYTE nStrLength = 0;

			fread(&nStrLength, sizeof(BYTE), 1, File);
			fread(m_FrameName, sizeof(char), nStrLength, File);
			m_FrameName[nStrLength] = '\0';

			for (; ;) {
				fread(&nStrLength, sizeof(BYTE), 1, File);
				fread(Token, sizeof(char), nStrLength, File);
				Token[nStrLength] = '\0';

				if (!strcmp(Token, "<BonesPerVertex>:")) {
					int m_nBonePerVertex = 0;
					fread(&m_nBonePerVertex, sizeof(int), 1, File);
				}

				else if (!strcmp(Token, "<Bounds>:")) {
					DirectX::XMFLOAT3 m_AABBCenter{}, m_AABBExtent{};
					fread(&m_AABBCenter, sizeof(DirectX::XMFLOAT3), 1, File);
					fread(&m_AABBExtent, sizeof(DirectX::XMFLOAT3), 1, File);
				}

				else if (!strcmp(Token, "<BoneNames>:")) {
					int m_nSkinningBone = 0;
					fread(&m_nSkinningBone, sizeof(int), 1, File);

					if (m_nSkinningBone > 0) {
						char(*m_SkinningBoneName)[64] = new char[m_nSkinningBone][64];
						GameObject **m_SkinningBoneFrameCache = new GameObject*[m_nSkinningBone];

						for (int i = 0; i < m_nSkinningBone; ++i) {
							fread(&nStrLength, sizeof(BYTE), 1, File);
							fread(m_SkinningBoneName[i], sizeof(char), nStrLength, File);
							m_SkinningBoneName[i][nStrLength] = '\0';

							m_SkinningBoneFrameCache[i] = NULL;
						}
					}
				}

				else if (!strcmp(Token, "<BoneOffsets>:")) {
					int m_nSkinningBone = 0;
					fread(&m_nSkinningBone, sizeof(int), 1, File);

					if (m_nSkinningBone > 0) {
						DirectX::XMFLOAT4X4 *m_BindPoseBoneOffset = new DirectX::XMFLOAT4X4[m_nSkinningBone];
						fread(m_BindPoseBoneOffset, sizeof(float), 16 * m_nSkinningBone, File);
					}
				}

				else if (!strcmp(Token, "<BoneWeights>:")) {
					//m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

					int m_nVertices = 0;
					fread(&m_nVertices, sizeof(int), 1, File);

					if (m_nVertices > 0) {
						DirectX::XMUINT4 *m_BoneIndice = new DirectX::XMUINT4[m_nVertices];
						DirectX::XMFLOAT4 *m_BoneWeight = new DirectX::XMFLOAT4[m_nVertices];

						fread(m_BoneIndice, sizeof(DirectX::XMUINT4), m_nVertices, File);
						/*m_BoneIndexBuffer = ::CreateBufferResource(Device, CommandList, m_BoneIndice, sizeof(XMUINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BoneIndexUploadBuffer);

						m_BoneIndexBufferView.BufferLocation = m_BoneIndexBuffer->GetGPUVirtualAddress();
						m_BoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
						m_BoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * m_nVertices;*/

						fread(m_BoneWeight, sizeof(DirectX::XMFLOAT4), m_nVertices, File);
						/*m_BoneWeightBuffer = ::CreateBufferResource(Device, CommandList, m_BoneWeight, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BoneWeightUploadBuffer);

						m_BoneWeightBufferView.BufferLocation = m_BoneWeightBuffer->GetGPUVirtualAddress();
						m_BoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
						m_BoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;*/
					}
				}

				else if (!strcmp(Token, "</SkinningInfo>")) {
					break;
				}
			}
		}

		else if (!strcmp(Token, "<Children>:")) {
			int nChild = ::ReadIntegerFile(File);

			if (nChild > 0)
				for (int i = 0; i < nChild; ++i) {
					GameObject *Child = GameObject::LoadModel2(File);
					//if (Child) GameObj->SetChild(Child);
				}
		}

		else if (!strcmp(Token, "</Frame>"))
			break;
	}

	return GameObj;
}

void GameObject::SetMesh(int MeshIndex, Mesh* ObjectMesh)
{
	m_Mesh[MeshIndex] = ObjectMesh;
}

void GameObject::SetShader(Shader* ObjectShader)
{
	m_Shader = ObjectShader;
}

void GameObject::SetTexture(Texture* ObjectTexture)
{
	m_Texture = ObjectTexture;
}

void GameObject::SetRight(DirectX::XMFLOAT3 Right)
{
	m_WorldPos._11 = Right.x;
	m_WorldPos._12 = Right.y;
	m_WorldPos._13 = Right.z;
}

void GameObject::SetUp(DirectX::XMFLOAT3 Up)
{
	m_WorldPos._21 = Up.x;
	m_WorldPos._22 = Up.y;
	m_WorldPos._23 = Up.z;
}

void GameObject::SetLook(DirectX::XMFLOAT3 Look)
{
	m_WorldPos._31 = Look.x;
	m_WorldPos._32 = Look.y;
	m_WorldPos._33 = Look.z;
}

void GameObject::SetPosition(DirectX::XMFLOAT3 Position)
{
	m_WorldPos._41 = Position.x;
	m_WorldPos._42 = Position.y;
	m_WorldPos._43 = Position.z;
}

void GameObject::SetTransformPos(DirectX::XMFLOAT4X4 TransformPos)
{
	m_TransformPos._11 = TransformPos._11, m_TransformPos._12 = TransformPos._12, m_TransformPos._13 = TransformPos._13, m_TransformPos._14 = TransformPos._14;
	m_TransformPos._21 = TransformPos._21, m_TransformPos._22 = TransformPos._22, m_TransformPos._23 = TransformPos._23, m_TransformPos._24 = TransformPos._24;
	m_TransformPos._31 = TransformPos._31, m_TransformPos._32 = TransformPos._32, m_TransformPos._33 = TransformPos._33, m_TransformPos._34 = TransformPos._34;
	m_TransformPos._41 = TransformPos._41, m_TransformPos._42 = TransformPos._42, m_TransformPos._43 = TransformPos._43, m_TransformPos._44 = TransformPos._44;
}

DirectX::XMFLOAT3 GameObject::GetRight()
{
	return DirectX::XMFLOAT3(m_WorldPos._11, m_WorldPos._12, m_WorldPos._13);
}

DirectX::XMFLOAT3 GameObject::GetUp()
{
	return DirectX::XMFLOAT3(m_WorldPos._21, m_WorldPos._22, m_WorldPos._23);
}

DirectX::XMFLOAT3 GameObject::GetLook()
{
	return DirectX::XMFLOAT3(m_WorldPos._31, m_WorldPos._32, m_WorldPos._33);
}

DirectX::XMFLOAT3 GameObject::GetPosition()
{
	return DirectX::XMFLOAT3(m_WorldPos._41, m_WorldPos._42, m_WorldPos._43);
}

void GameObject::Rotate(DirectX::XMFLOAT3 Angle)
{
	DirectX::XMMATRIX Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Angle.x), DirectX::XMConvertToRadians(Angle.y), DirectX::XMConvertToRadians(Angle.z));
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixMultiply(Rotate, DirectX::XMLoadFloat4x4(&m_WorldPos)));
}

void GameObject::Animate(float ElapsedTime)
{

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

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	for (int i = 0; i < m_MeshCount; ++i) {
		if (m_Texture != nullptr) m_Texture->Render(CommandList, i);
		if (m_Mesh[i] != nullptr) m_Mesh[i]->Render(CommandList);
	}
}