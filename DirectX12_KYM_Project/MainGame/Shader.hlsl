// ������Ʈ�� 1���� �ؽ�ó�� ����
Texture2D Texture : register(t0);
// Base + Detail - 2���� �ؽ�ó�� ����
Texture2D BaseTexture : register(t1);
Texture2D DetailTexture : register(t2);
// Bin Model�� ����ϴ� �ؽ�ó
Texture2D BinTexture : register(t3);

// ī�޶� and ���� ��ȯ�� ����� ����
cbuffer Camera : register(b0)
{
    matrix CameraPos : packoffset(c0);
    matrix ProjectionPos : packoffset(c4);
    float3 CamwPos : packoffset(c8);
};

// ������ ������ �޴� ������Ʈ ����
struct MATERIAL
{
    float4 m_Ambient;
    float4 m_Diffuse;
    float4 m_Specular;
    float4 m_Emissive;
};

// ������Ʈ�� ���� ��ȯ �� ������ ����� ����
cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
    MATERIAL Material : packoffset(c4);
    int Damaged : packoffset(c8);
};

// ���� ������ ����� ���� ���� - ����ü, ����, ���� ���� ������ ����
#define LIGHT_COUNT 16

#define DIRECTIONAL_LIGHT 0
#define SPOT_LIGHT 1
#define POINT_LIGHT 2

struct LIGHT
{
    float4 m_Ambient;
    float4 m_Diffuse;
    float4 m_Specular;
    float4 m_Emissive;
    float3 m_Direction;
    int m_Type;
    bool m_Active;
};

// ���� ��꿡 ���Ǵ� ����
cbuffer Lighting : register(b4)
{    
    LIGHT Lights : packoffset(c0);
};

// ���⼺ ���� - ���� ������ �¾� ������ ���
float4 DirectionalLight(float3 oNormal)
{
    float3 ToLight = Lights.m_Direction;
    float DiffuseFactor = dot(ToLight, oNormal);
    
    // �� ������ ���� ������ ������ ������Ʈ�� ������ ����
    float4 Color = (Lights.m_Ambient * Material.m_Ambient) + (Lights.m_Diffuse * DiffuseFactor * Material.m_Diffuse) + (Lights.m_Specular * Material.m_Specular) + (Lights.m_Emissive * Material.m_Emissive);
    
    return Color;
}

// ������ ������ ���߾� ������Ʈ ������ ���
float4 CalLighting(float3 oPosition, float3 oNormal)
{
    float3 CamerawPosition = float3(CamwPos.x, CamwPos.y, CamwPos.z);
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    
    if (Lights.m_Active == true)
    {
        if (Lights.m_Type == DIRECTIONAL_LIGHT)
        {
            Color += DirectionalLight(oNormal);
        }
    }
    
    return Color;
}

// �ؽ�ó�� ���� ������ ����� ���÷�
SamplerState Sampler : register(s0);

// No Texture
struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_Output
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VS_Output VS(VS_Input Input)
{
    VS_Output Output;
        
    Output.position = mul(mul(mul(float4(Input.position, 1.f), WorldPos), CameraPos), ProjectionPos);
    Output.color = Input.color;
    
    return Output;
}

float4 PS(VS_Output Input) : SV_TARGET
{
    return Input.color;
}

// �ؽ�ó ������ �ϴ� ������Ʈ
struct TextureVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
};

struct TextureVS_Output
{
    float4 position : SV_Position;
    float2 uv : UV;
};

struct InstancingTextureVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    uint kind : KIND;
    float4x4 TransformPos : TRANSFORMPOS;
};

struct InstancingTextureVS_Output
{
    float3 position : POSITION;
    float2 uv : UV;
    uint kind : KIND;
};

// ---------- Vertex Shader ---------- //
TextureVS_Output UserInterfaceVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(float4(Input.position, 1.f), WorldPos);
    Output.uv = Input.uv;
    
    return Output;
}

TextureVS_Output TextureVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(mul(mul(float4(Input.position, 1.f), WorldPos), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

InstancingTextureVS_Output InstancingTextureVS(InstancingTextureVS_Input Input)
{
    InstancingTextureVS_Output Output;
    
    Output.position = mul(float4(Input.position, 1.f), Input.TransformPos);
    Output.uv = Input.uv;
    Output.kind = Input.kind;
    
    return Output;
}

// ---------- Geometry Shader ---------- //
struct InstancingTextureGS_Output
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

[maxvertexcount(4)]
void BillboardGS(point InstancingTextureVS_Output Input[1], inout TriangleStream<InstancingTextureGS_Output> OutputStream)
{
    // ������ ��ü�� ī�޶� �ٶ� ������ ����
    float3 Up = float3(0.f, 1.f, 0.f);
    float3 Look = normalize(CamwPos - Input[0].position);
    float3 Right = cross(Up, Look);
    
    float Width = 0.f, Height = 0.f;
    
    // ��ü�� ������ ���� ũ�⸦ ����
    switch (Input[0].kind)
    {
        case 4: // Grass
            Width = 50.f, Height = 10.f;
            break;
        
        case 5: // Tree
            Width = 75.f, Height = 100.f;
            break;
    }
    
    // ������ ����� Right, Up, Look�� ���� ������ ��ġ�� ����
    float4 MeshVertex[4];
    MeshVertex[0] = float4(Input[0].position + (Width * Right) - (Height * Up), 1.f);
    MeshVertex[1] = float4(Input[0].position + (Width * Right) + (Height * Up), 1.f);
    MeshVertex[2] = float4(Input[0].position - (Width * Right) - (Height * Up), 1.f);
    MeshVertex[3] = float4(Input[0].position - (Width * Right) + (Height * Up), 1.f);
    
    float2 MeshUv[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };
    
    InstancingTextureGS_Output Output;
    
    for (int i = 0; i < 4; ++i)
    {
        Output.position = mul(mul(MeshVertex[i], CameraPos), ProjectionPos);
        Output.uv = MeshUv[i];
        OutputStream.Append(Output);
    }
}

// ---------- Pixel Shader ---------- //
float4 TexturePS(TextureVS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
    clip(Color.a - 0.1f);
    
    return Color;
}

float4 InstancingTexturePS(InstancingTextureGS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
    clip(Color.a - 0.1f);
    
    return Color;
}

// Terrain�� ��� - 2���� �ؽ�ó�� ������ �����ϰ� �׸��ڿ� ����� RGBA�� ���
struct TerrainVS_Input
{
    float3 position : POSITION;
    float2 uv0 : UV0;
    float2 uv1 : UV1;
    float4 color : COLOR;
};

struct TerrainVS_Output
{
    float4 position : SV_Position;
    float2 uv0 : UV0;
    float2 uv1 : UV1;
    float4 color : COLOR;
};

TerrainVS_Output TerrainVS(TerrainVS_Input Input)
{
    TerrainVS_Output Output;
    
    Output.position = mul(mul(mul(float4(Input.position, 1.f), WorldPos), CameraPos), ProjectionPos);
    Output.uv0 = Input.uv0;
    Output.uv1 = Input.uv1;
    Output.color = Input.color;
    
    return Output;
}

float4 TerrainPS(TerrainVS_Output Input) : SV_TARGET
{
    float4 BaseUv = BaseTexture.Sample(Sampler, Input.uv0);
    float4 DetailUv = DetailTexture.Sample(Sampler, Input.uv1);
    float4 Color = Input.color * saturate((BaseUv * 0.5f) + (DetailUv * 0.5f));
    
    return Color;
}

// Binary Load Model�� ��� - Skin�� ���� ������ ����
#define BONE_PER_VERTEX 4
#define SKINNED_ANIMATION_BONE 128

cbuffer BoneOffset : register(b2)
{
    float4x4 BoneOffsetPos[SKINNED_ANIMATION_BONE];
};

cbuffer BoneTransform : register(b3)
{
    float4x4 BoneTransformPos[SKINNED_ANIMATION_BONE];
}

struct LoadedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct InstancingLoadedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4x4 TransformPos : TRANSFORMPOS;
};

struct SkinnedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    uint4 boneindex : BONEINDEX;
    float4 boneweight : BONEWEIGHT;
};

struct InstancingSkinnedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    uint4 boneindex : BONEINDEX;
    float4 boneweight : BONEWEIGHT;
    float4x4 TransformPos : TRANSFORMPOS;
};

struct LoadedVS_Output
{
    float4 position : SV_Position;
    float3 positionw : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

LoadedVS_Output LoadedVS(LoadedVS_Input Input)
{
    LoadedVS_Output Output;
    
    Output.positionw = mul(float4(Input.position, 1.f), WorldPos).xyz;
    Output.normal = mul(Input.normal, (float3x3) WorldPos);
    Output.tangent = mul(Input.tangent, (float3x3) WorldPos);
    Output.bitangent = mul(Input.bitangent, (float3x3) WorldPos);
    
    Output.position = mul(mul(float4(Output.positionw, 1.f), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

LoadedVS_Output InstanceLoadedVS(InstancingLoadedVS_Input Input)
{
    LoadedVS_Output Output;
    
    Output.positionw = mul(float4(Input.position, 1.f), Input.TransformPos).xyz;
    Output.normal = mul(Input.normal, (float3x3) Input.TransformPos);
    Output.tangent = mul(Input.tangent, (float3x3) Input.TransformPos);
    Output.bitangent = mul(Input.bitangent, (float3x3) Input.TransformPos);
    
    Output.position = mul(mul(float4(Output.positionw, 1.f), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

LoadedVS_Output SkinnedVS(SkinnedVS_Input Input)
{
    LoadedVS_Output Output;
    
    Output.positionw = float3(0.f, 0.f, 0.f);
    Output.normal = float3(0.f, 0.f, 0.f);
    Output.tangent = float3(0.f, 0.f, 0.f);
    Output.bitangent = float3(0.f, 0.f, 0.f);
    matrix VertexToBoneWorld;
    
    for (int i = 0; i < BONE_PER_VERTEX; ++i)
    {
        VertexToBoneWorld = mul(BoneOffsetPos[Input.boneindex[i]], BoneTransformPos[Input.boneindex[i]]);
        Output.positionw += Input.boneweight[i] * mul(float4(Input.position, 1.f), VertexToBoneWorld).xyz;
        Output.normal += Input.boneweight[i] * mul(Input.normal, (float3x3) VertexToBoneWorld);
        Output.tangent += Input.boneweight[i] * mul(Input.tangent, (float3x3) VertexToBoneWorld);
        Output.bitangent += Input.boneweight[i] * mul(Input.bitangent, (float3x3) VertexToBoneWorld);
    }
    
    Output.position = mul(mul(float4(Output.positionw, 1.f), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

LoadedVS_Output InstanceSkinnedVS(InstancingSkinnedVS_Input Input)
{
    LoadedVS_Output Output;
    
    Output.positionw = float3(0.f, 0.f, 0.f);
    Output.normal = float3(0.f, 0.f, 0.f);
    Output.tangent = float3(0.f, 0.f, 0.f);
    Output.bitangent = float3(0.f, 0.f, 0.f);
    matrix VertexToBoneWorld = Input.TransformPos;
    
    for (int i = 0; i < BONE_PER_VERTEX; ++i)
    {
        //VertexToBoneWorld = Input.TransformPos + mul(BoneOffsetPos[Input.boneindex[i]], BoneTransformPos[Input.boneindex[i]]);
        Output.positionw += Input.boneweight[i] * mul(float4(Input.position, 1.f), VertexToBoneWorld).xyz;
        Output.normal += Input.boneweight[i] * mul(Input.normal, (float3x3) VertexToBoneWorld);
        Output.tangent += Input.boneweight[i] * mul(Input.tangent, (float3x3) VertexToBoneWorld);
        Output.bitangent += Input.boneweight[i] * mul(Input.bitangent, (float3x3) VertexToBoneWorld);
    }
    
    Output.position = mul(mul(float4(Output.positionw, 1.f), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

float4 LoadedPS(LoadedVS_Output Input) : SV_TARGET
{
    // 1. Bin���� ���� �ؽ�ó�� ����
    float4 AlbedoTexture = BinTexture.Sample(Sampler, Input.uv);
    
    // 2. Bin ���� ���� Normal Map�� ������ ������ ����
    // *�������� ����ϴ� ���� �����Ƿ� ��� ����
    
    // 3-1. Normal Map�� ������ ���� tangent, bitangent, normal ������ �븻 ���� ����
    // *�������� ����ϴ� ���� �����Ƿ� ��� ����
    
    // 3-2. Normal Map�� ������ VS���� �о�� normal ������ ����
    float3 Normalw = normalize(Input.normal);
    
    // 4. Scene�� ���⼺ ����� ���� ���� ������ ����Ͽ� ���� ���� ���
    float4 Illumination = CalLighting(Input.positionw, Normalw);
    
    // ������Ʈ �ǰ� ��, ���� ����
    if (Damaged & 0x02)
    {
        AlbedoTexture.x = 1.f;
    }
    
    // 5. ���� �ؽ�ó�� ���� ���� �����ؼ� ���� ������ ����
    return lerp(AlbedoTexture, Illumination, 0.5f);
}