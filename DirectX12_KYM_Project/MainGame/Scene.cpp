#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력-조립(IA) 단계 허용
	RootSignatureDesc.pParameters = nullptr;
	RootSignatureDesc.NumParameters = 0;
	RootSignatureDesc.pStaticSamplers = nullptr;
	RootSignatureDesc.NumStaticSamplers = 0;

	ID3DBlob *RootSignatureBlob = nullptr;
	ID3DBlob *ErrorBlob = nullptr;

	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_RootSignature);

	if (RootSignatureBlob != nullptr) RootSignatureBlob->Release();
	if (ErrorBlob != nullptr) ErrorBlob->Release();
}

void Scene::CreateShader(ID3D12Device* Device)
{
	m_Shader = new Shader();

	m_Shader->CreateShader(Device, m_RootSignature);
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	m_Shader->Render(CommandList);

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CommandList->DrawInstanced(3, 1, 0, 0);
}
