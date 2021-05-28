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

	m_MeshCount = 1;
	m_Mesh = new Mesh*[m_MeshCount];

	Mesh *UsingMesh = new Mesh();
	UsingMesh->CreateMesh(Device, CommandList, 10.f);
	SetMesh(0, UsingMesh);

	Shader *UsingShader = new Shader();
	UsingShader->CreateShader(Device, RootSignature);
	SetShader(UsingShader);

	// Player 오브젝트가 바라보는 화면을 플레이어에게 보여줄 수 있게 Camera 생성
	m_Camera = new Camera();
	m_Camera->CreateCamera();
}

void Player::Render(ID3D12GraphicsCommandList* CommandList)
{
	m_Camera->Update(CommandList, this);

	UpdateShaderCode(CommandList);

	if (m_Shader != nullptr) m_Shader->Render(CommandList);
	if (m_Texture != nullptr) m_Texture->Render(CommandList, 0);
	for (int i = 0; i < m_MeshCount; ++i) if (m_Mesh[i] != nullptr) m_Mesh[i]->Render(CommandList);
}