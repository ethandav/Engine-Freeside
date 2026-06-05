struct PointShadowData
{
    float4x4 faceViewProjection[6];
    float farPlane;
    float3 _padding;
};

StructuredBuffer<PointShadowData> gPointShadows : register(t4);

float SamplePointShadowPCF(
    TextureCube<float> shadowCube,
    float3 sampleDir,
    float currentDepth,
    float bias,
    float diskRadius)
{
    float3 offsets[6] =
    {
        float3(1.0f, 0.0f, 0.0f),
        float3(-1.0f, 0.0f, 0.0f),
        float3(0.0f, 1.0f, 0.0f),
        float3(0.0f, -1.0f, 0.0f),
        float3(0.0f, 0.0f, 1.0f),
        float3(0.0f, 0.0f, -1.0f)
    };

    float visibility = 0.0f;

    // Center sample.
    float sampledDepth = shadowCube.SampleLevel(gLinearSampler, sampleDir, 0).r;
    visibility += currentDepth - bias <= sampledDepth ? 1.0f : 0.0f;

    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        float3 dir = normalize(sampleDir + offsets[i] * diskRadius);
        sampledDepth = shadowCube.SampleLevel(gLinearSampler, dir, 0).r;

        visibility += currentDepth - bias <= sampledDepth ? 1.0f : 0.0f;
    }

    return visibility / 7.0f;
}

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

    float diskRadius = 0.0025f;

    float visibility = SamplePointShadowPCF(
        shadowCube,
        sampleDir,
        currentDepth,
        gShadowBias,
        diskRadius);

    return lerp(1.0f, visibility, gShadowStrength);
}