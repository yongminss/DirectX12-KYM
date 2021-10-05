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
};
// ���� ��ȯ�� ����� ����
cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
};

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
        
    Output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
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

TextureVS_Output UserInterfaceVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(float4(Input.position, 1.0f), WorldPos);
    Output.uv = Input.uv;
    
    return Output;
}

TextureVS_Output TextureVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

float4 TexturePS(TextureVS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
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
    
    Output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
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
};

struct SkinedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    uint4 boneindex : BONEINDEX;
    float4 boneweight : BONEWEIGHT;
};

struct LoadedVS_Output
{
    float4 position : SV_Position;
    float2 uv : UV;
};

LoadedVS_Output LoadedVS(LoadedVS_Input Input)
{
    LoadedVS_Output Output;
            
    Output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}

float4 LoadedPS(LoadedVS_Output Input) : SV_TARGET
{
    float4 Color = BinTexture.Sample(Sampler, Input.uv);
    
    return Color;
}

LoadedVS_Output SkinedVS(SkinedVS_Input Input)
{
    LoadedVS_Output Output;
    
    matrix VertexToBoneWorld;
    float3 BonePosition = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < BONE_PER_VERTEX; ++i)
    {
        VertexToBoneWorld = mul(BoneOffsetPos[Input.boneindex[i]], BoneTransformPos[Input.boneindex[i]]);
        BonePosition += Input.boneweight[i] * mul(float4(Input.position, 1.f), VertexToBoneWorld).xyz;
    }
    
    Output.position = mul(mul(float4(BonePosition, 1.f), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    return Output;
}