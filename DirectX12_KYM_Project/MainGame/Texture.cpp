#include "stdafx.h"
#include "Texture.h"

#include "DDSTextureLoader12.h"
#include "Scene.h"


Texture::Texture()
{

}

Texture::~Texture()
{
	for (int i = 0; i < m_TextureCount; ++i) m_TextureBuffer[i]->Release();
	for (int i = 0; i < m_TextureCount; ++i) m_UploadTextureBuffer[i]->Release();

	if (m_ShaderResourceViewDescriptorHeap != nullptr) m_ShaderResourceViewDescriptorHeap->Release();
}

void Texture::CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, wchar_t* TextureName, int Kind, int TextureCount, int RootParameterIndex)
{
	m_DescriptorHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_RootParameterIndex = RootParameterIndex;
	m_TextureCount = TextureCount;
	m_TextureBuffer = new ID3D12Resource*[m_TextureCount];
	m_UploadTextureBuffer = new ID3D12Resource*[m_TextureCount];

	// (wchar_t *)로 넘어온 TextureName을 (char *)로 형 변환
	int TextureNameSize = WideCharToMultiByte(CP_ACP, 0, TextureName, -1, NULL, 0, NULL, NULL);

	m_TextureName = new char[TextureNameSize];
	WideCharToMultiByte(CP_ACP, 0, TextureName, -1, m_TextureName, TextureNameSize, 0, 0);

	// 1. 오브젝트에 매핑할 텍스처 파일(DDS)을 읽고 리소스를 생성
	CreateTextureBuffer(Device, CommandList, TextureName, Kind);

	// 2. 텍스처 매핑을 위해 ShaderResource View 생성
	// ShaderResource View를 만들기 위해 필요한 Descriptor Heap은 Scene에서 생성했으므로 ShaderResource View를 생성
	Scene::CreateShaderResourceView(Device, this, RootParameterIndex);
}

void Texture::CreateTextureBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, wchar_t* TextureName, int Kind)
{
	for (int i = 0; i < m_TextureCount; ++i) {
		std::unique_ptr<uint8_t[]> TextureFileData{};
		std::vector<D3D12_SUBRESOURCE_DATA> SubresourceData{};
		DirectX::DDS_ALPHA_MODE TextureFileAlphaMode = DirectX::DDS_ALPHA_MODE_UNKNOWN;
		bool ActiveCubeMap = false;

		switch (Kind) {
		case T_TITLESCREEN:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_TitleScreen.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_SELECTION:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_Bar.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GAMEMANUAL:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GameManual.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_TERRAIN:
		{
			switch (i) {
			case 0:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Terrain_Base.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 1:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Terrain_Detail.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;
			}
		}
		break;

		case T_SKYBOX:
		{
			switch (i) {
			case 0:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Front.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 1:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Back.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 2:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Left.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 3:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Right.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 4:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Top.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 5:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/SkyBox_Bottom.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;
			}
		}
		break;

		case T_BILLBOARDTREE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Billboard_Tree.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_WALL:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Billboard_Wall.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_TREE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Billboard_Tree.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_ITEMHP:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Item_Hp.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_FLAME:
		{
			switch (i) {
			case 0:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Flame_Fire.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 1:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Flame_Noise.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;

			case 2:
			{
				DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Flame_Alpha.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
					TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
			}
			break;
			}
		}
		break;

		case T_SMOKE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Effect_Smoke.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_POWDER:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Effect_Smoke.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_SPARK:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Effect_Spark.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_SIGNAL:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Effect_Signal.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_HEADSHOT:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/Effect_Headshot.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_HPBAR:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_Bar.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_HPGAUGE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_HpGauge.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_AIM:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_Aim.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_NUMBERS:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_Numbers.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GAMEOVER:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GameOver.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GAMEEND:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GameEnd.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_ENTERFIRE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_EnterFire.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_ENTERMONSTER:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_EnterMonster.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GUIDEAREA:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GuideArea.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GUIDENONE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GuideNone.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GUIDEFIRE:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GuideFire.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		case T_GUIDEMONSTER:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, L"Texture/UI_GuideMonster.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;

		default:
		{
			DirectX::LoadDDSTextureFromFileEx(Device, TextureName, 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer[i],
				TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
		}
		break;
		}

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;

		UINT64 Size = GetRequiredIntermediateSize(m_TextureBuffer[i], 0, (UINT)SubresourceData.size());

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = Size;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;

		Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&m_UploadTextureBuffer[i]);

		UpdateSubresources(CommandList, m_TextureBuffer[i], m_UploadTextureBuffer[i], 0, 0, (UINT)SubresourceData.size(), &SubresourceData[0]);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		ResourceBarrier.Transition.pResource = m_TextureBuffer[i];
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
}

void Texture::CreateDescriptorHeap(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescriptorHeapDesc.NumDescriptors = m_TextureCount;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescriptorHeapDesc.NodeMask = 0;
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_ShaderResourceViewDescriptorHeap);
}

void Texture::CreateShaderResourceView(ID3D12Device* Device)
{
	m_CpuDescriptorHandle = m_ShaderResourceViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_GpuDescriptorHandle = m_ShaderResourceViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	for (int i = 0; i < m_TextureCount; ++i) {
		D3D12_RESOURCE_DESC ResourceDesc = m_TextureBuffer[i]->GetDesc();

		D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = ResourceDesc.Format;
		ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = -1;
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.f;

		Device->CreateShaderResourceView(m_TextureBuffer[i], &ShaderResourceViewDesc, m_CpuDescriptorHandle);
		m_CpuDescriptorHandle.ptr += m_DescriptorHandleIncrementSize;
	}
}

void Texture::SetRootArgument(int Index, int RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptorHandle)
{
	if (m_RootArgument == nullptr)
		m_RootArgument = new ROOTARGUMENT[m_TextureCount];

	m_RootArgument[Index].m_RootParameterIndex = RootParameterIndex;
	m_RootArgument[Index].m_GpuDescriptorHandle = GpuDescriptorHandle;
}

void Texture::Render(ID3D12GraphicsCommandList* CommandList, int TextureIndex)
{
	CommandList->SetGraphicsRootDescriptorTable(m_RootArgument[TextureIndex].m_RootParameterIndex, m_RootArgument[TextureIndex].m_GpuDescriptorHandle);
}