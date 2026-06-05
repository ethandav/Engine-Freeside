struct PointShadowData
{
    float4x4 faceViewProjection[6];
    float farPlane;
    float3 _padding;
};

StructuredBuffer<PointShadowData> gPointShadows : register(t4);

float ComputePointShadowFactor(
    float3 worldPosition,
    float3 lightPosition,
    int shadowDataIndex,
    int shadowTextureDescriptorIndex)
{
    if (shadowDataIndex < 0 || shadowDataIndex >= (int) gPointShadowCount)
    {
        return 1.0f;
    }

    if (shadowTextureDescriptorIndex < 0)
    {
        return 1.0f;
    }

    PointShadowData shadowData = gPointShadows[shadowDataIndex];

    float3 lightToPixel = worldPosition - lightPosition;
    float distanceToLight = length(lightToPixel);

    if (distanceToLight <= 0.0001f)
    {
        return 1.0f;
    }

    if (distanceToLight > shadowData.farPlane)
    {
        return 1.0f;
    }

    float3 sampleDir = lightToPixel / distanceToLight;

    TextureCube<float> shadowCube =
        ResourceDescriptorHeap[NonUniformResourceIndex(shadowTextureDescriptorIndex)];

    float currentDepth = distanceToLight / shadowData.farPlane;
    float sampledDepth = shadowCube.SampleLevel(gLinearSampler, sampleDir, 0).r;

    float visibility =
        currentDepth - gShadowBias <= sampledDepth
        ? 1.0f
        : 0.0f;

    return lerp(1.0f, visibility, gShadowStrength);
}