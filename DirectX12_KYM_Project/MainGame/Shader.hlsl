// 조명 계산에 사용하는 모델의 재질
struct MATERIAL
{
    float4 m_Ambient;
    float4 m_Diffuse;
    float4 m_Specular;
    float4 m_Emissive;
};

// 오브젝트의 카메라 변환 and 투영 변환에 사용하는 버퍼
cbuffer Camera : register(b0)
{
    matrix CameraPos : packoffset(c0);
    matrix ProjectionPos : packoffset(c4);
    float3 CamwPos : packoffset(c8);
};

// 오브젝트의 변환(ex. 월드 좌표, 색상, uv 등)에 사용하는 버퍼
cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
    MATERIAL Material : packoffset(c4);
    float4 ChangeTexcoords : packoffset(c8);
};

// 조명 계산에 사용하는 구조체와 버퍼 생성
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

// 게임 내에서 태양 빛으로 사용되는 방향성 조명
float4 DirectionalLight(int Index, float3 oNormal)
{
    float3 ToLight = Lights[Index].m_Direction;
    float DiffuseFactor = dot(ToLight, oNormal);
        
    // 색 결정을 위해 조명의 재질과 오브젝트의 재질을 곱함
        float4 Color =
    (Lights[Index].m_Ambient * Material.m_Ambient) +
    (Lights[Index].m_Diffuse * DiffuseFactor * Material.m_Diffuse) +
    (Lights[Index].m_Specular * Material.m_Specular) +
    (Lights[Index].m_Emissive * Material.m_Emissive);
    
    return Color;
}

// 태양 빛 외의 다른 오브젝트의 조명 효과 (ex. 불꽃 효과 or 그림자 등)
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
        
        // 거리에 따른 감쇄 값 설정
        float AttenuationFactor = 1.f / dot(Lights[Index].m_Attenuation, float3(1.f, Distance, Distance * Distance));
        
        Color *= AttenuationFactor;
    }
    
    return Color;
}

// 조명의 종류에 따라 오브젝트의 재질과 계산하여 색상을 결정
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


// Texture를 사용하지 않는 오브젝트의 Shader 생성
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


// 1개의 Texture를 가진 오브젝트에 사용
Texture2D Texture : register(t0);

// Texture의 색상을 결정하는 Sampler
SamplerState Sampler : register(s0);

// Texture를 사용하는 오브젝트의 Shader 생성
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


// 같은 Texture를 1번만 생성하기 위해 Instance 오브젝트 생성
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

// Geometry Shader를 사용해서 오브젝트를 변환
struct InstancingTextureGS_Output
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

[maxvertexcount(4)]
void BillboardGS(point InstancingTextureVS_Output Input[1], inout TriangleStream<InstancingTextureGS_Output> OutputStream)
{
    // 빌보드 객체가 카메라를 바라 보도록 설정
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
    
    // 위에서 계산한 Right, Up, Look을 통해 정점의 위치를 설정
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
    
    // 객체의 종류에 따라 크기를 설정
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


// Flame Object가 사용 - 3개의 텍스처 매핑
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


// Base + Detail - 2개의 텍스처를 매핑
Texture2D BaseTexture : register(t4);
Texture2D DetailTexture : register(t5);

// Terrain에 사용 - 2개의 텍스처로 디테일 매핑하고 그림자에 사용할 RGBA값 사용
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


// Binary Load Model에 사용 - Skin의 존재 유무로 구분
#define BONE_PER_VERTEX 4
#define SKINNED_ANIMATION_BONE 128

// Bin Model이 사용하는 텍스처
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
    // 1. Bin파일 모델의 텍스처를 읽음
    float4 AlbedoTexture = BinTexture.Sample(Sampler, Input.uv);
    
    // 2. Bin 파일 모델이 Normal Map을 가지고 있으면 읽음
    // *아직까진 사용하는 모델은 없으므로 잠시 생략
    
    // 3-1. Normal Map이 있으면 모델의 tangent, bitangent, normal 정보로 노말 값을 설정
    // *아직까진 사용하는 모델은 없으므로 잠시 생략
    
    // 3-2. Normal Map이 없으면 VS에서 읽어온 normal 값으로 설정
    float3 Normalw = normalize(Input.normal);
    
    // 4. 특정 지역(ex. Fire, Monster Area)에 입장한 경우, 텍스처 색상 변경
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
    
    // 5. 조명과 모델 재질에 따라 텍스처 색상 변경
    float4 Illumination = CalLighting(Input.positionw, Normalw);
    
    // 6. 피격 시, 색상 변화
    if (1.f == ChangeTexcoords.x) AlbedoTexture.x = 1.f;
    
    // 7. 텍스처의 최종 결과 계산
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

// Binary Load Model의 인스턴싱을 위해 사용 - 보류
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