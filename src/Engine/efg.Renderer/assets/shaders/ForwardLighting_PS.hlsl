#include "include\geometry.hlsli"

cbuffer MaterialCB : register(b3)
{
    float4 BaseColor;
    float4 Specular;
    float2 uvScale;
};

cbuffer PointLightMetadata : register(b4)
{
    uint gPointLightCount;
    float3 _pointLightPadding;
};

cbuffer DirectionalLightMetadata : register(b2)
{
    uint gDirectionalLightCount;
    float3 _directionalLightPadding;
};

cbuffer ShadowMetadataCB : register(b5)
{
    uint gDirectionalShadowCount;
    uint gPointShadowCount;
    float gShadowBias;
    float gShadowStrength;
};

struct PointLight
{
    float4 positionAndRadius;
    float4 colorAndIntensity;
    int shadowDataIndex;
    int shadowTextureDescriptorIndex;
    float3 _padding;
};

struct DirectionalLight
{
    float4 lightDirectionAndIntensity;
    float4 lightColor;
    int shadowDataIndex;
    int shadowTextureDescriptorIndex;
    float3 _padding;
};

struct DirectionalShadowData
{
    float4x4 lightViewProjection;
};

struct PointShadowData
{
    float4x4 faceViewProjection[6];
    float farPlane;
    float3 _padding;
};

StructuredBuffer<PointLight> gPointLights : register(t1);
Texture2D gBaseColorTexture : register(t2);
StructuredBuffer<DirectionalLight> gDirectionalLights : register(t4);
StructuredBuffer<DirectionalShadowData> gDirectionalShadows : register(t5);
StructuredBuffer<PointShadowData> gPointShadows : register(t6);
SamplerState gLinearSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

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

float3 AccumulatePointLights(float3 worldPos, float3 normal, float3 viewDir, float4 baseColor)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < gPointLightCount; ++i)
    {
        PointLight light = gPointLights[i];
        float3 lightPos = light.positionAndRadius.xyz;
        float radius = max(light.positionAndRadius.w, 0.0001f);
        float3 lightColor = light.colorAndIntensity.rgb;
        float intensity = light.colorAndIntensity.w;
        float3 toLight = lightPos - worldPos;
        float distanceToLight = length(toLight);
        float3 lightDir = toLight / max(distanceToLight, 0.0001f);
        float attenuation = saturate(1.0f - distanceToLight / radius);
        attenuation *= attenuation;
        float shadowFactor = ComputePointShadowFactor(worldPos, lightPos, light.shadowDataIndex, light.shadowTextureDescriptorIndex);
        float ndotl = saturate(dot(normal, lightDir));
        float3 diffuse = baseColor.rgb * lightColor * ndotl * intensity * attenuation;
        float3 halfVector = normalize(lightDir + viewDir);
        float specularAmount = pow(saturate(dot(normal, halfVector)), Specular.y);
        float3 specular = lightColor * specularAmount * Specular.x * intensity * attenuation;

        result += shadowFactor * (diffuse + specular);
    }

    return result;
}

float3 AccumulateDirectionalLights(float3 worldPos, float3 normal, float3 viewDir, float4 materialColor)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < gDirectionalLightCount; ++i)
    {
        DirectionalLight light = gDirectionalLights[i];
        float shadowFactor = ComputeDirectionalShadowFactor(worldPos, light.shadowDataIndex, light.shadowTextureDescriptorIndex);
        float3 lightDir = normalize(-light.lightDirectionAndIntensity.xyz);
        float ndotl = saturate(dot(normal, lightDir));
        float3 diffuse = materialColor.rgb * light.lightColor.rgb * ndotl * light.lightDirectionAndIntensity.w;
        float3 halfVector = normalize(lightDir + viewDir);
        float specularAmount = pow(saturate(dot(normal, halfVector)), Specular.y);
        float3 specular = light.lightColor.rgb * specularAmount * Specular.x * light.lightDirectionAndIntensity.w;

        result += shadowFactor * (diffuse + specular);
    }

    return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * uvScale;
    float4 sampledBaseColor = gBaseColorTexture.Sample(gLinearSampler, uv);
    float3 normal = normalize(input.normalWS);
    float3 viewDir = normalize(ViewPosition.xyz - input.worldPosition);
    float3 ambient = sampledBaseColor.rgb * 0.1f;
    float3 directionalLighting = AccumulateDirectionalLights(input.worldPosition, normal, viewDir, sampledBaseColor);
    float3 pointLighting = AccumulatePointLights(input.worldPosition, normal, viewDir, sampledBaseColor);
    float3 finalColor = ambient + directionalLighting + pointLighting;

    return float4(finalColor, BaseColor.a);
}