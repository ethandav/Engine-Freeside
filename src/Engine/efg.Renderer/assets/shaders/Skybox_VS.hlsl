struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

cbuffer SkyboxCB : register(b0)
{
    float4x4 ViewProjection;
    uint SkyboxTextureDescriptorIndex;
    float3 _padding;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.direction = input.position;
    float4 clip = mul(ViewProjection, float4(input.position, 1.0f));

    // Force depth to far plane.
    output.position = clip.xyww;

    return output;
}