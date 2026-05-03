struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
    VSOutput output;

    float2 positions[3] =
    {
        float2(0.0f, 0.5f),
        float2(0.5f, -0.5f),
        float2(-0.5f, -0.5f)
    };

    float4 colors[3] =
    {
        float4(0.0f, 0.0f, 1.0f, 1.0f),
        float4(0.0f, 0.0f, 1.0f, 1.0f),
        float4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    output.position = float4(positions[vertexId], 0.0f, 1.0f);
    output.color = colors[vertexId];

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
}