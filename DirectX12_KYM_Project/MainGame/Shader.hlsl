cbuffer Camera : register(b0)
{
    matrix CameraPos : packoffset(c0);
    matrix ProjectionPos : packoffset(c4);
};

cbuffer GameObject : register(b1)
{
    matrix WorldPos : packoffset(c0);
};

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
    VS_Output output;
        
    output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
    output.color = Input.color;
    
    return output;
}

float4 PS(VS_Output Input) : SV_TARGET
{
    return Input.color;
}

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

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
    TextureVS_Output output;
    
    output.position = mul(float4(Input.position, 1.0f), WorldPos);
    output.uv = Input.uv;
    
    return output;
}

TextureVS_Output TextureVS(TextureVS_Input Input)
{
    TextureVS_Output output;
    
    output.position = mul(mul(mul(float4(Input.position, 1.0f), WorldPos), CameraPos), ProjectionPos);
    output.uv = Input.uv;
    
    return output;
}

float4 TexturePS(TextureVS_Output Input) : SV_TARGET
{
    float4 color = Texture.Sample(Sampler, Input.uv);
    
    return color;
}