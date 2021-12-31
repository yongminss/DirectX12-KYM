#include "stdafx.h"

ID3D12Resource* CreateBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* VertexData, int Size, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES ResourceState, ID3D12Resource *UploadBuffer)
{
	ID3D12Resource *Buffer = nullptr;

	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = HeapType;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

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

	D3D12_RESOURCE_STATES ResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) ResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;

	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, ResourceStates, nullptr, __uuidof(ID3D12Resource), (void**)&Buffer);

	if (VertexData != nullptr) {
		switch (HeapType) {
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&UploadBuffer);

			D3D12_RANGE Range = { 0 };
			void* BufferDataBegin = nullptr;
			UploadBuffer->Map(0, &Range, (void**)&BufferDataBegin);
			memcpy(BufferDataBegin, VertexData, Size);
			UploadBuffer->Unmap(0, nullptr);

			CommandList->CopyResource(Buffer, UploadBuffer);

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 리소스 사용 변화를 나타내는 전이 장벽
			ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			ResourceBarrier.Transition.pResource = Buffer;
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			ResourceBarrier.Transition.StateAfter = ResourceState;
			ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			CommandList->ResourceBarrier(1, &ResourceBarrier);
		}
		break;

		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE Range = { 0 };
			void* BufferDataBegin = nullptr;
			UploadBuffer->Map(0, &Range, (void**)&BufferDataBegin);
			memcpy(BufferDataBegin, VertexData, Size);
			UploadBuffer->Unmap(0, nullptr);
		}
		break;
		}
	}
	return Buffer;
}