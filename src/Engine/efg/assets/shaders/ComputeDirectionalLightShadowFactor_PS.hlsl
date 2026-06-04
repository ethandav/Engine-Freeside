struct DirectionalShadowData
{
    float4x4 lightViewProjection;
};

StructuredBuffer<DirectionalShadowData> gDirectionalShadows : register(t3);

float ComputeDirectionalShadowFactor(float3 worldPosition, int shadowDataIndex, int shadowTextureDescriptorIndex)
{
    if (shadowDataIndex < 0 || shadowDataIndex >= (int) gDirectionalShadowCount)
    {
        return 1.0f;
    }

    DirectionalShadowData shadowData = gDirectionalShadows[shadowDataIndex];
    
    float4 lightClip = mul(shadowData.lightViewProjection, float4(worldPosition, 1.0f));
    float3 lightNdc = lightClip.xyz / lightClip.w;

    float2 shadowUV;
    shadowUV.x = lightNdc.x * 0.5f + 0.5f;
    shadowUV.y = -lightNdc.y * 0.5f + 0.5f;

    float currentDepth = lightNdc.z;

    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f || shadowUV.y > 1.0f ||
        currentDepth < 0.0f || currentDepth > 1.0f)
    {
        return 1.0f;
    }

    Texture2D<float> shadowMap = ResourceDescriptorHeap[NonUniformResourceIndex(shadowTextureDescriptorIndex)];
    float visibility = shadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, currentDepth - gShadowBias);

    return lerp(1.0f, visibility, gShadowStrength);
}