// 오브젝트에 1개의 텍스처를 매핑
Texture2D Texture : register(t0);
// Base + Detail - 2개의 텍스처를 매핑
Texture2D BaseTexture : register(t1);
Texture2D DetailTexture : register(t2);

// 카메라 and 투영 변환에 사용할 버퍼
cbuffer Camera : register(b0)
{
    matrix CameraPos : packoffset(c0);
    matrix ProjectionPos : packoffset(c4);
};
// 월드 변환에 사용할 버퍼
cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
};

// 텍스처의 색상 결정에 사용할 샘플러
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

// 텍스처 매핑을 하는 오브젝트
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

// Terrain에 사용 - 2개의 텍스처로 디테일 매핑하고 그림자에 사용할 RGBA값 사용
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

// -
struct LoadedVS_Input
{
    float3 position : POSITION;
};

struct LoadedVS_Output
{
    float4 position : SV_Position;
};

LoadedVS_Output LoadedVS(LoadedVS_Input Input)
{
    LoadedVS_Output Output;
            
    Output.position = mul(mul(float4(Input.position, 1.0f), CameraPos), ProjectionPos);
    
    return Output;
}

float4 LoadedPS(LoadedVS_Output Input) : SV_TARGET
{
    return float4(1.f, 1.f, 1.f, 1.f);
}