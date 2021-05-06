#include "stdafx.h"
#include "Player.h"


Player::Player()
{

}

Player::~Player()
{
	if (m_Camera != nullptr) delete m_Camera;
}

void Player::CreateGameObject(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* RootSignature)
{
	DirectX::XMStoreFloat4x4(&m_WorldPos, DirectX::XMMatrixIdentity());

	Mesh *UsingMesh = new Mesh();
	UsingMesh->CreateMesh(Device, CommandList, 10.f);
	SetMesh(UsingMesh);

	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	// Player ������Ʈ�� �ٶ󺸴� ȭ���� �÷��̾�� ������ �� �ְ� Camera ����
	m_Camera = new Camera();
	m_Camera->CreateCamera();
}

void Player::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, this);

	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	if (m_Texture != nullptr) m_Texture->Render(CommandList);
	if (m_Mesh != nullptr) m_Mesh->Render(CommandList);
}