#include "ComputePointLightShadowFactor_PS.hlsl"

cbuffer PointLightMetadata : register(b4)
{
    uint gPointLightCount;
    float3 _pointLightPadding;
};

struct PointLight
{
    float4 positionAndRadius;
    float4 colorAndIntensity;
    int shadowDataIndex;
    int shadowTextureDescriptorIndex;
    float3 _padding;
};

StructuredBuffer<PointLight> gPointLights : register(t1);

float3 AccumulatePointLights(float3 worldPos, float3 normal, float3 viewDir, float4 baseColor, float metallic, float roughness)
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
        float3 diffuseColor = baseColor.rgb * (1.0f - metallic);
        float3 diffuse = diffuseColor * lightColor * ndotl * intensity * attenuation;
        float3 halfVector = normalize(lightDir + viewDir);
        float specularStrength = lerp(0.04f, 1.0f, metallic);
        float shininess = lerp(256.0f, 8.0f, roughness);
        float specularAmount = pow(saturate(dot(normal, halfVector)), shininess);
        float3 specular = lightColor * specularAmount * specularStrength * intensity;

        result += shadowFactor * (diffuse + specular);
    }

    return result;
}