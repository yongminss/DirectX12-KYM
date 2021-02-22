float4 VS(uint nVertexID : SV_VertexID)
{
    float4 output;
    
    if (nVertexID == 0)
        output = float4(0.f, 0.5f, 0.5f, 1.f);
    else if (nVertexID == 1)
        output = float4(0.f, -0.5f, 0.5f, 1.f);
    else if (nVertexID == 2)
        output = float4(-0.f, -0.5f, 0.5f, 1.f);
    
    return output;
}

float4 PS(float4 input : SV_Position) : SV_TARGET
{
    return float4(1.f, 0.f, 0.f, 1.f);
}