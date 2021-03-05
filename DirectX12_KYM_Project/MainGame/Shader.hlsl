struct VS_Input
{
    float3 position : POSITION;
    float4 diffuse : COLOR;
};

struct VS_Output
{
    float4 position : SV_Position;
    float4 diffuse : COLOR;
};

VS_Output VS(VS_Input Input)
{
    VS_Output output;
    
    output.position = float4(Input.position, 1.f);
    output.diffuse = Input.diffuse;
    
    return output;
}

float4 PS(VS_Output Input) : SV_TARGET
{
    return Input.diffuse;
}