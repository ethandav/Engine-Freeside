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

TextureCube<float4> GetSkyboxTexture(uint descriptorIndex)
{
    return ResourceDescriptorHeap[NonUniformResourceIndex(descriptorIndex)];
}

SamplerState gLinearClampSampler : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    TextureCube<float4> skyboxTexture = ResourceDescriptorHeap[NonUniformResourceIndex(SkyboxTextureDescriptorIndex)];

    float3 dir = normalize(input.direction);

    return skyboxTexture.Sample(gLinearClampSampler, dir);
}