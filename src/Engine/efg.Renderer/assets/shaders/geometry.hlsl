cbuffer CameraCB : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 World;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    float4 worldPosition = mul(World, float4(input.position, 1.0f));
    output.position = mul(ViewProjection, worldPosition);
    output.color = input.color;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
}