#include "stdafx.h"
#include "Texture.h"
#include "DDSTextureLoader12.h"


Texture::Texture()
{

}

Texture::~Texture()
{
	for (int i = 0; i < m_TextureCount; ++i) m_TextureBuffer[i]->Release();
	for (int i = 0; i < m_TextureCount; ++i) m_UploadTextureBuffer[i]->Release();

	if (m_ShaderResourceViewDescriptorHeap != nullptr) m_ShaderResourceViewDescriptorHeap->Release();
}

void Texture::CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind, int TextureCount)
{
	m_DescriptorHandleIncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_TextureCount = TextureCount;
	m_TextureBuffer = new ID3D12Resource*[m_TextureCount];
	m_UploadTextureBuffer = new ID3D12Resource*[m_TextureCount];

	// 1. 오브젝트에 매핑할 텍스처 파일(DDS)을 읽고 리소스를 생성
	CreateTextureBuffer(Device, CommandList, Kind);
	// 2. ShaderResource View를 만들기 위해 Descriptor Heap을 생성
	CreateDescriptorHeap(Device);
	// 3. 텍스처 매핑을 위해 ShaderResource View 생성
	CreateShaderResourceView(Device);
}

void Texture::CreateTextureBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind)
{
	for (int i = 0; i < m_TextureCount; ++i, ++Kind) {
		std::unique_ptr<uint8_t[]> TextureFileData{};
		std::vector<D3D12_SUBRESOURCE_DATA> SubresourceData{};
		DirectX::DDS_ALPHA_MODE TextureFileAlphaMode = DirectX::DDS_ALPHA_MODE_UNKNOWN;
		bool ActiveCubeMap = false;

		switch (Kind) {
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

void Texture::Render(ID3D12GraphicsCommandList* CommandList, int TextureIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE TargetGpuDescriptorHandle;
	TargetGpuDescriptorHandle.ptr = m_GpuDescriptorHandle.ptr + (TextureIndex * m_DescriptorHandleIncrementSize);

	if (TextureIndex == 0) CommandList->SetDescriptorHeaps(1, &m_ShaderResourceViewDescriptorHeap);
	CommandList->SetGraphicsRootDescriptorTable(2, TargetGpuDescriptorHandle);
}