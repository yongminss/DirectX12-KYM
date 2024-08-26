// ���� ��꿡 ����ϴ� ���� ����
struct MATERIAL
{
    float4 m_Ambient;
    float4 m_Diffuse;
    float4 m_Specular;
    float4 m_Emissive;
};

// ������Ʈ�� ī�޶� ��ȯ and ���� ��ȯ�� ����ϴ� ����
cbuffer Camera : register(b0)
{
    matrix CameraPos : packoffset(c0);
    matrix ProjectionPos : packoffset(c4);
    float3 CamwPos : packoffset(c8);
};

// ������Ʈ�� ��ȯ(ex. ���� ��ǥ, ����, uv ��)�� ����ϴ� ����
cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
    MATERIAL Material : packoffset(c4);
    float4 ChangeTexcoords : packoffset(c8);
};

// ���� ��꿡 ����ϴ� ����ü�� ���� ����
#define LIGHT_MAX 16

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
    float m_Range;
    float3 m_Position;
    int m_Type;
    bool m_Active;
    float3 m_Attenuation;
};

cbuffer Lighting : register(b2)
{
    LIGHT Lights[LIGHT_MAX];
    int LightCount;
};

// ���� ������ �¾� ������ ���Ǵ� ���⼺ ����
float4 DirectionalLight(int Index, float3 oNormal)
{
    float3 ToLight = Lights[Index].m_Direction;
    float DiffuseFactor = dot(ToLight, oNormal);
        
    // �� ������ ���� ������ ������ ������Ʈ�� ������ ����
        float4 Color =
    (Lights[Index].m_Ambient * Material.m_Ambient) +
    (Lights[Index].m_Diffuse * DiffuseFactor * Material.m_Diffuse) +
    (Lights[Index].m_Specular * Material.m_Specular) +
    (Lights[Index].m_Emissive * Material.m_Emissive);
    
    return Color;
}

// �¾� �� ���� �ٸ� ������Ʈ�� ���� ȿ�� (ex. �Ҳ� ȿ�� or �׸��� ��)
float4 PointLight(int Index, float3 oPosition, float3 oNormal)
{
    float3 ToLight = Lights[Index].m_Position - oPosition;
    float Distance = length(ToLight);
    ToLight /= Distance;
    float DiffuseFactor = dot(ToLight, oNormal);
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    
    if (Distance <= Lights[Index].m_Range)
    {
        Color =
    (Lights[Index].m_Ambient * Material.m_Ambient) +
    (Lights[Index].m_Diffuse * DiffuseFactor * Material.m_Diffuse) +
    (Lights[Index].m_Specular * Material.m_Specular) +
    (Lights[Index].m_Emissive * Material.m_Emissive);
        
        // �Ÿ��� ���� ���� �� ����
        float AttenuationFactor = 1.f / dot(Lights[Index].m_Attenuation, float3(1.f, Distance, Distance * Distance));
        
        Color *= AttenuationFactor;
    }
    
    return Color;
}

// ������ ������ ���� ������Ʈ�� ������ ����Ͽ� ������ ����
float4 CalLighting(float3 oPosition, float3 oNormal)
{
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int i = 0; i < LightCount; ++i)
    {
        if (Lights[i].m_Active == true)
        {
            if (Lights[i].m_Type == DIRECTIONAL_LIGHT)
            {
                Color += DirectionalLight(i, oNormal);
            }
            else if (Lights[i].m_Type == POINT_LIGHT)
            {
                Color += PointLight(i, oPosition, oNormal);
            }
        }
    }
    return Color;
}


// Texture�� ������� �ʴ� ������Ʈ�� Shader ����
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
// -----


// 1���� Texture�� ���� ������Ʈ�� ���
Texture2D Texture : register(t0);

// Texture�� ������ �����ϴ� Sampler
SamplerState Sampler : register(s0);

// Texture�� ����ϴ� ������Ʈ�� Shader ����
struct TextureVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    uint kind : KIND;
};

struct TextureVS_Output
{
    float4 position : SV_Position;
    float2 uv : UV;
    uint kind : KIND;
};

TextureVS_Output UserInterfaceVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(float4(Input.position, 1.f), WorldPos);
    Output.uv = Input.uv + float2(ChangeTexcoords.x, ChangeTexcoords.y);
    Output.kind = Input.kind;
    
    return Output;
}

TextureVS_Output TextureVS(TextureVS_Input Input)
{
    TextureVS_Output Output;
    
    Output.position = mul(mul(mul(float4(Input.position, 1.f), WorldPos), CameraPos), ProjectionPos);
    Output.uv = Input.uv + float2(ChangeTexcoords.x, ChangeTexcoords.y);
    Output.kind = Input.kind;
    
    return Output;
}

float4 TexturePS(TextureVS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
    if (0.f != ChangeTexcoords.z)
    {
        float ChangeRange = ChangeTexcoords.z / 2.f;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    if (0.f != ChangeTexcoords.w)
    {
        float ChangeRange = ChangeTexcoords.w / 2.f;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    clip(Color.a - 0.1f);
    
    return Color;
}

float4 BlendTexturePS(TextureVS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
    switch (Input.kind)
    {
        case 10: // Smoke
            break;
        
        case 11: // Powder
            Color += float4(0.75f, 0.75f, 0.75f, 0.f);
            break;
        
        case 19: // GameOver Screen
        case 20: // GameEnd Screen
            Color.w = ChangeTexcoords.x / 1000.f;
            break;
    }
    clip(Color.a - 0.1f);
    
    return Color;
}
// -----


// ���� Texture�� 1���� �����ϱ� ���� Instance ������Ʈ ����
struct InstancingTextureVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    uint kind : KIND;
    float4x4 transformPos : TRANSFORMPOS;
};

struct InstancingTextureVS_Output
{
    float3 position : POSITION;
    float2 uv : UV;
    uint kind : KIND;
};

InstancingTextureVS_Output InstancingTextureVS(InstancingTextureVS_Input Input)
{
    InstancingTextureVS_Output Output;
    
    Output.position = mul(float4(Input.position, 1.f), Input.transformPos);
    Output.uv = Input.uv;
    Output.kind = Input.kind;
    
    return Output;
}

// Geometry Shader�� ����ؼ� ������Ʈ�� ��ȯ
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
    
    uint kind = Input[0].kind;
    
    switch (kind)
    {        
        case 5: // Tree
            Width = 75.f, Height = 100.f;
            break;
        
        case 6: // Wall
        case 7:
        case 8:
        case 9:
            Width = 100.f, Height = 100.f;
            break;
    }
    
    // ������ ����� Right, Up, Look�� ���� ������ ��ġ�� ����
    float4 MeshVertex[4];
        
    switch (kind)
    {
        case 5:
            MeshVertex[0] = float4(Input[0].position + (Width * Right) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * Right) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * Right) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * Right) + (Height * Up), 1.f);
            break;
        
        case 6:
            MeshVertex[0] = float4(Input[0].position + (Width * float3(-1.f, 0.f, 0.f)) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * float3(-1.f, 0.f, 0.f)) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * float3(-1.f, 0.f, 0.f)) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * float3(-1.f, 0.f, 0.f)) + (Height * Up), 1.f);
            break;
        
        case 7:
            MeshVertex[0] = float4(Input[0].position + (Width * float3(1.f, 0.f, 0.f)) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * float3(1.f, 0.f, 0.f)) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * float3(1.f, 0.f, 0.f)) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * float3(1.f, 0.f, 0.f)) + (Height * Up), 1.f);
            break;
        
        case 8:
            MeshVertex[0] = float4(Input[0].position + (Width * float3(0.f, 0.f, -1.f)) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * float3(0.f, 0.f, -1.f)) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * float3(0.f, 0.f, -1.f)) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * float3(0.f, 0.f, -1.f)) + (Height * Up), 1.f);
            break;
        
        case 9:
            MeshVertex[0] = float4(Input[0].position + (Width * float3(0.f, 0.f, 1.f)) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * float3(0.f, 0.f, 1.f)) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * float3(0.f, 0.f, 1.f)) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * float3(0.f, 0.f, 1.f)) + (Height * Up), 1.f);
            break;
        
        default:
            MeshVertex[0] = float4(Input[0].position + (Width * Right) - (Height * Up), 1.f);
            MeshVertex[1] = float4(Input[0].position + (Width * Right) + (Height * Up), 1.f);
            MeshVertex[2] = float4(Input[0].position - (Width * Right) - (Height * Up), 1.f);
            MeshVertex[3] = float4(Input[0].position - (Width * Right) + (Height * Up), 1.f);
            break;
    }
    
    // ��ü�� ������ ���� ũ�⸦ ����
    float2 MeshUv[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };
    
    InstancingTextureGS_Output Output;
    
    for (int i = 0; i < 4; ++i)
    {
        Output.position = mul(mul(MeshVertex[i], CameraPos), ProjectionPos);
        Output.uv = MeshUv[i];
        OutputStream.Append(Output);
    }
}

float4 InstancingTexturePS(InstancingTextureGS_Output Input) : SV_TARGET
{
    float4 Color = Texture.Sample(Sampler, Input.uv);
    
    if (0.f != ChangeTexcoords.z)
    {
        float ChangeRange = ChangeTexcoords.z / 2.f;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    if (0.f != ChangeTexcoords.w)
    {
        float ChangeRange = ChangeTexcoords.w / 2.f;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    clip(Color.a - 0.1f);
    
    return Color;
}
// -----


// Flame Object�� ��� - 3���� �ؽ�ó ����
Texture2D FireTexture : register(t1);
Texture2D NoiseTexture : register(t2);
Texture2D AlphaTexture : register(t3);

SamplerState FlameSampler : register(s1);

struct NOISE
{
    float m_FrameTime;
    float3 m_ScrollSpeed;
    float3 m_Scale;
};

cbuffer Noising : register(b3)
{
    NOISE Noise : packoffset(c0);
};

struct DISTORTION
{
    float2 m_Distortion1;
    float2 m_Distortion2;
    float2 m_Distortion3;
    float m_Scale;
    float m_Bias;
};

cbuffer Distortion : register(b4)
{
    DISTORTION Distort : packoffset(c0);
};

struct FlameVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
};

struct FlameVS_Output
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};

FlameVS_Output FlameVS(FlameVS_Input Input)
{
    FlameVS_Output Output;
    
    Output.position = mul(mul(mul(float4(Input.position, 1.f), WorldPos), CameraPos), ProjectionPos);
    Output.uv = Input.uv;
    
    Output.texCoords1 = Input.uv * Noise.m_Scale.x;
    Output.texCoords1.y = Output.texCoords1.y + (Noise.m_FrameTime * Noise.m_ScrollSpeed.x);
    
    Output.texCoords2 = Input.uv * Noise.m_Scale.y;
    Output.texCoords2.y = Output.texCoords2.y + (Noise.m_FrameTime * Noise.m_ScrollSpeed.y);
    
    Output.texCoords3 = Input.uv * Noise.m_Scale.z;
    Output.texCoords3.y = Output.texCoords3.y + (Noise.m_FrameTime * Noise.m_ScrollSpeed.z);
    
    return Output;
}

float4 FlamePS(FlameVS_Output Input) : SV_TARGET
{
    float4 Noise1 = NoiseTexture.Sample(Sampler, Input.texCoords1);
    float4 Noise2 = NoiseTexture.Sample(Sampler, Input.texCoords2);
    float4 Noise3 = NoiseTexture.Sample(Sampler, Input.texCoords3);
    
    Noise1 = (Noise1 - 0.5f) * 2.f;
    Noise2 = (Noise2 - 0.5f) * 2.f;
    Noise3 = (Noise3 - 0.5f) * 2.f;
    
    Noise1.xy = Noise1.xy * Distort.m_Distortion1.xy;
    Noise2.xy = Noise2.xy * Distort.m_Distortion2.xy;
    Noise3.xy = Noise3.xy * Distort.m_Distortion3.xy;
    
    float4 FinalNoise = Noise1 + Noise2 + Noise3;
    
    float Perturb = ((1.f - Input.uv.y) * Distort.m_Scale) + Distort.m_Bias;
    
    float2 NoiseCoords;
    NoiseCoords.xy = (FinalNoise.xy * Perturb) + Input.uv.xy;
    
    float4 FireColor = FireTexture.Sample(FlameSampler, NoiseCoords.xy);
    float4 AlphaColor = AlphaTexture.Sample(FlameSampler, NoiseCoords.xy);
    
    FireColor.a = AlphaColor;
    clip(FireColor.a - 0.1f);
    
    return FireColor;
}
// -----


// Base + Detail - 2���� �ؽ�ó�� ����
Texture2D BaseTexture : register(t4);
Texture2D DetailTexture : register(t5);

// Terrain�� ��� - 2���� �ؽ�ó�� ������ �����ϰ� �׸��ڿ� ����� RGBA�� ���
struct TerrainVS_Input
{
    float3 position : POSITION;
    float2 uv0 : UV0;
    float2 uv1 : UV1;
};

struct TerrainVS_Output
{
    float4 position : SV_Position;
    float3 positionw : POSITION;
    float2 uv0 : UV0;
    float2 uv1 : UV1;
};

TerrainVS_Output TerrainVS(TerrainVS_Input Input)
{
    TerrainVS_Output Output;
    
    Output.positionw = mul(float4(Input.position, 1.f), WorldPos).xyz;
    Output.uv0 = Input.uv0;
    Output.uv1 = Input.uv1;
    
    Output.position = mul(mul(float4(Output.positionw, 1.f), CameraPos), ProjectionPos);

    return Output;
}

float4 TerrainPS(TerrainVS_Output Input) : SV_TARGET
{
    float4 BaseUv = BaseTexture.Sample(Sampler, Input.uv0);
    float4 DetailUv = DetailTexture.Sample(Sampler, Input.uv1);
    
    float4 Color = saturate(BaseUv + DetailUv);
    
    if (0.f != ChangeTexcoords.z)
    {
        float ChangeRange = ChangeTexcoords.z;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    if (0.f != ChangeTexcoords.w)
    {
        float ChangeRange = ChangeTexcoords.w;
        Color -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    
    float3 Normalw = float3(BaseUv.x, BaseUv.y, BaseUv.z);
    normalize(Normalw);
    
    float4 Illumination = CalLighting(Input.positionw, Normalw);
    
    return lerp(Color, Illumination, 0.5f);
}
// -----


// Binary Load Model�� ��� - Skin�� ���� ������ ����
#define BONE_PER_VERTEX 4
#define SKINNED_ANIMATION_BONE 128

// Bin Model�� ����ϴ� �ؽ�ó
Texture2D BinTexture : register(t6);

cbuffer BoneOffset : register(b5)
{
    float4x4 BoneOffsetPos[SKINNED_ANIMATION_BONE];
};

cbuffer BoneTransform : register(b6)
{
    float4x4 BoneTransformPos[SKINNED_ANIMATION_BONE];
};

struct LoadedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
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
    
    // 4. Ư�� ����(ex. Fire, Monster Area)�� ������ ���, �ؽ�ó ���� ����
    if (0.f != ChangeTexcoords.z)
    {
        float ChangeRange = ChangeTexcoords.z;
        AlbedoTexture -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    if (0.f != ChangeTexcoords.w)
    {
        float ChangeRange = ChangeTexcoords.w;
        AlbedoTexture -= float4(ChangeRange, ChangeRange, ChangeRange, 0.f);
    }
    
    // 5. ����� �� ������ ���� �ؽ�ó ���� ����
    float4 Illumination = CalLighting(Input.positionw, Normalw);
    
    // 6. �ǰ� ��, ���� ��ȭ
    if (1.f == ChangeTexcoords.x) AlbedoTexture.x = 1.f;
    
    // 7. �ؽ�ó�� ���� ��� ���
    return lerp(AlbedoTexture, Illumination, 0.5f);
}

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

// Binary Load Model�� �ν��Ͻ��� ���� ��� - ����
struct InstancingLoadedVS_Input
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4x4 TransformPos : TRANSFORMPOS;
};

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

LoadedVS_Output InstanceSkinnedVS(InstancingSkinnedVS_Input Input)
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