#include "ComputeDirectionalLightShadowFactor_PS.hlsl"

cbuffer DirectionalLightMetadata : register(b2)
{
    uint gDirectionalLightCount;
    float3 _directionalLightPadding;
};

struct DirectionalLight
{
    float4 lightDirectionAndIntensity;
    float4 lightColor;
    int shadowDataIndex;
    int shadowTextureDescriptorIndex;
    float3 _padding;
};

StructuredBuffer<DirectionalLight> gDirectionalLights : register(t2);

float3 AccumulateDirectionalLights(float3 worldPos, float3 normal, float3 viewDir, float4 materialColor, float metallic, float roughness)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < gDirectionalLightCount; ++i)
    {
        DirectionalLight light = gDirectionalLights[i];
        float shadowFactor = ComputeDirectionalShadowFactor(worldPos, light.shadowDataIndex, light.shadowTextureDescriptorIndex);
        float3 lightDir = normalize(-light.lightDirectionAndIntensity.xyz);
        float ndotl = saturate(dot(normal, lightDir));
        float3 diffuseColor = materialColor.rgb * (1.0f - metallic);
        float3 diffuse = diffuseColor * light.lightColor.rgb * ndotl * light.lightDirectionAndIntensity.w;
        float3 halfVector = normalize(lightDir + viewDir);
        float specularStrength = lerp(0.04f, 1.0f, metallic);
        float shininess = lerp(256.0f, 8.0f, roughness);
        float specularAmount = pow(saturate(dot(normal, halfVector)), shininess);
        float3 specular = light.lightColor.rgb * specularAmount * specularStrength * light.lightDirectionAndIntensity.w;

        result += shadowFactor * (diffuse + specular);
    }

    return result;
}