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