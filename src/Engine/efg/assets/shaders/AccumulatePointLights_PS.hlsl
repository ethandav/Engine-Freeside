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

    float3 N = normalize(normal);
    float3 V = normalize(viewDir);

    float3 albedo = baseColor.rgb;
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

    for (uint i = 0; i < gPointLightCount; ++i)
    {
        PointLight light = gPointLights[i];

        float3 lightPos = light.positionAndRadius.xyz;
        float radius = max(light.positionAndRadius.w, 0.0001f);

        float3 lightColor = light.colorAndIntensity.rgb;
        float intensity = light.colorAndIntensity.w;

        float3 toLight = lightPos - worldPos;
        float distanceToLight = length(toLight);
        float3 L = toLight / max(distanceToLight, 0.0001f);

        float NdotL = saturate(dot(N, L));
        if (NdotL <= 0.0f)
        {
            continue;
        }

        float x = saturate(distanceToLight / radius);
        float attenuation = saturate(1.0f - x * x);
        attenuation *= attenuation;

        float3 radiance = lightColor * intensity * attenuation;

        float shadowFactor = ComputePointShadowFactor(worldPos, lightPos, light.shadowDataIndex, light.shadowTextureDescriptorIndex);

        float3 H = normalize(V + L);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

        float NdotV = saturate(dot(N, V));

        float3 numerator = NDF * G * F;
        float denominator = 4.0f * max(NdotV, 0.0001f) * max(NdotL, 0.0001f);
        float3 specular = numerator / max(denominator, 0.0001f);

        float3 kS = F;
        float3 kD = 1.0f - kS;

        // Metals have no diffuse term.
        kD *= 1.0f - metallic;

        float3 diffuse = kD * albedo / PI;

        result += shadowFactor * (diffuse + specular) * radiance * NdotL;
    }

    return result;
}