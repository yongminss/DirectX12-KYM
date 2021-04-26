#include "stdafx.h"
#include "Texture.h"
#include "DDSTextureLoader12.h"


Texture::Texture()
{

}

Texture::~Texture()
{
	if (m_TextureBuffer != nullptr) m_TextureBuffer->Release();
	if (m_UploadTextureBuffer != nullptr) m_UploadTextureBuffer->Release();
}

void Texture::CreateTexture(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, int Kind)
{
	std::unique_ptr<uint8_t[]> TextureFileData{};
	std::vector<D3D12_SUBRESOURCE_DATA> SubresourceData{};
	DirectX::DDS_ALPHA_MODE TextureFileAlphaMode = DirectX::DDS_ALPHA_MODE_UNKNOWN;
	bool ActiveCubeMap = false;

	switch (Kind) {
	case 0:
	{
		DirectX::LoadDDSTextureFromFileEx(Device, L"blahblah.dds", 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, &m_TextureBuffer,
			TextureFileData, SubresourceData, &TextureFileAlphaMode, &ActiveCubeMap);
	}
	break;

	default:
		std::cout << "Unknown DDS File!\n";
		return;
	}

	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	UINT64 Size = GetRequiredIntermediateSize(m_TextureBuffer, 0, (UINT)SubresourceData.size());

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

	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&m_UploadTextureBuffer);

	UpdateSubresources(CommandList, m_TextureBuffer, m_UploadTextureBuffer, 0, 0, (UINT)SubresourceData.size(), &SubresourceData[0]);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_TextureBuffer;
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);
}