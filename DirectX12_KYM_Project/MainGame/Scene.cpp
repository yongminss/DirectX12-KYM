#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{

}

Scene::~Scene()
{
	if (m_RootSignature != nullptr) m_RootSignature->Release();

	if (m_GameObject != nullptr) delete m_GameObject;
}

void Scene::CreateRootSignature(ID3D12Device* Device)
{
	D3D12_ROOT_PARAMETER RootParameter[2];
	ZeroMemory(&RootParameter, sizeof(RootParameter));
	// Camera
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[0].Constants.ShaderRegister = 0;
	RootParameter[0].Constants.RegisterSpace = 0;
	RootParameter[0].Constants.Num32BitValues = 32;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// GameObject
	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameter[1].Constants.ShaderRegister = 1;
	RootParameter[1].Constants.RegisterSpace = 0;
	RootParameter[1].Constants.Num32BitValues = 16;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �Է�-����(IA) �ܰ� ���
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumParameters = 2;
	RootSignatureDesc.pStaticSamplers = nullptr;
	RootSignatureDesc.NumStaticSamplers = 0;

	ID3DBlob *RootSignatureBlob = nullptr;
	ID3DBlob *ErrorBlob = nullptr;

	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_RootSignature);

	if (RootSignatureBlob != nullptr) RootSignatureBlob->Release();
	if (ErrorBlob != nullptr) ErrorBlob->Release();
}

void Scene::CreateScene(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// ���ҽ��� �׷��Ƚ� ���������ο� �����ϴ� ������ �ϴ� RootSignature ����
	CreateRootSignature(Device);

	// ���� ���带 Scene���� �����ֱ� ���� Camera ����
	m_Camera = new Camera();
	m_Camera->CreateCamera();

	// ���� ���忡 �����ϴ� Game Object ����
	m_GameObject = new GameObject();
	m_GameObject->CreateGameObject(Device, CommandList, m_RootSignature);
}

void Scene::Render(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_RootSignature);

	if (m_Camera != nullptr) m_Camera->SetViewportAndScissorRect(CommandList);
	if (m_Camera != nullptr) m_Camera->UpdateShaderCode(CommandList);

	if (m_GameObject != nullptr) m_GameObject->Render(CommandList);
}