struct PointShadowData
{
    float4x4 faceViewProjection[6];
    float farPlane;
    float3 _padding;
};

StructuredBuffer<PointShadowData> gPointShadows : register(t4);

uint GetCubeFaceIndex(float3 dir)
{
    float3 absDir = abs(dir);

    if (absDir.x >= absDir.y && absDir.x >= absDir.z)
    {
        return dir.x >= 0.0f ? 0 : 1;
    }

    if (absDir.y >= absDir.x && absDir.y >= absDir.z)
    {
        return dir.y >= 0.0f ? 2 : 3;
    }

    return dir.z >= 0.0f ? 4 : 5;
}

float ComputePointShadowFactor(float3 worldPosition, float3 lightPosition, int shadowDataIndex, int shadowTextureDescriptorIndex)
{
    if (shadowDataIndex < 0 || shadowDataIndex >= (int) gPointShadowCount)
    {
        return 1.0f;
    }

    float3 lightToPixel = worldPosition - lightPosition;
    float3 sampleDir = normalize(lightToPixel);
    uint faceIndex = GetCubeFaceIndex(sampleDir);
    
    PointShadowData shadowData = gPointShadows[shadowDataIndex];

    float4 lightClip = mul(
        shadowData.faceViewProjection[faceIndex],
        float4(worldPosition, 1.0f)
    );

    float3 lightNdc = lightClip.xyz / lightClip.w;
    float currentDepth = lightNdc.z;

    if (currentDepth < 0.0f || currentDepth > 1.0f)
    {
        return 1.0f;
    }

    TextureCube<float> shadowCube = ResourceDescriptorHeap[NonUniformResourceIndex(shadowTextureDescriptorIndex)];

    float sampledDepth = shadowCube.Sample(gLinearSampler, sampleDir);
    float visibility = currentDepth - gShadowBias <= sampledDepth ? 1.0f : 0.0f;

    return lerp(1.0f, visibility, gShadowStrength);
}