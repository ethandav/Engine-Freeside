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
    float2 _padding;
};

StructuredBuffer<DirectionalLight> gDirectionalLights : register(t2);

float3 AccumulateDirectionalLights(
    float3 worldPos,
    float3 normal,
    float3 viewDir,
    float4 baseColor,
    float metallic,
    float roughness)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    float3 N = normalize(normal);
    float3 V = normalize(viewDir);

    float3 albedo = baseColor.rgb;
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

    for (uint i = 0; i < gDirectionalLightCount; ++i)
    {
        DirectionalLight light = gDirectionalLights[i];

        float3 L = normalize(-light.lightDirectionAndIntensity.xyz);
        float3 H = normalize(V + L);

        float NdotL = saturate(dot(N, L));
        if (NdotL <= 0.0f)
        {
            continue;
        }

        float NdotV = saturate(dot(N, V));

        float3 radiance = light.lightColor.rgb * light.lightDirectionAndIntensity.w;

        float shadowFactor = ComputeDirectionalShadowFactor(worldPos, light.shadowDataIndex, light.shadowTextureDescriptorIndex);
        shadowFactor = lerp(1.0f, shadowFactor, gShadowStrength);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

        float3 numerator = NDF * G * F;
        float denominator = 4.0f * max(NdotV, 0.0001f) * max(NdotL, 0.0001f);

        float3 specular = numerator / max(denominator, 0.0001f);

        float3 kS = F;
        float3 kD = 1.0f - kS;

        // Metals should not have diffuse reflection.
        kD *= 1.0f - metallic;

        float3 diffuse = kD * albedo / PI;

        result += shadowFactor * (diffuse + specular) * radiance * NdotL;
    }

    return result;
}