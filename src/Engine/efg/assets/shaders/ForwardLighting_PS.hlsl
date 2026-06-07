SamplerState gLinearSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

cbuffer ShadowMetadataCB : register(b5)
{
    uint gDirectionalShadowCount;
    uint gPointShadowCount;
    float gShadowBias;
    float gShadowStrength;
};

#include "Materials.hlsli"
#include "AccumulateDirectionalLights_PS.hlsl"
//#include "AccumulatePointLights_PS.hlsl"
#include "PBRAccumulatePointLights_PS.hlsl"
#include "ApplyParallaxOcclusionMapping_PS.hlsl"
#include "ApplyNormalMap_PS.hlsl"

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tangentWS : TEXCOORD3;
};

cbuffer CameraCB : register(b0)
{
    float4 ViewPosition;
    float4x4 ViewProjection;
};

void BuildTBN(float3 normalWS, float4 tangentWS, out float3 T, out float3 B, out float3 N)
{
    N = normalize(normalWS);
    T = normalize(tangentWS.xyz - N * dot(tangentWS.xyz, N));
    float sign = tangentWS.w < 0.0f ? -1.0f : 1.0f;
    B = normalize(cross(N, T)) * sign;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * UvTransform.xy + UvTransform.zw;
    float3 viewDir = normalize(ViewPosition.xyz - input.worldPosition);

    float3 T, B, N;
    BuildTBN(input.normalWS, input.tangentWS, T, B, N);
    float3x3 TBN = float3x3(T, B, N);
    float3 viewDirTS = mul(viewDir, transpose(TBN));
    
    
    if (MaterialFlags & MaterialFlag_HasHeightTexture)
    {
        uv = ApplyParallaxOcclusionMapping(uv, viewDirTS);
    }

    float4 baseColor = BaseColorFactor;

    if (MaterialFlags & MaterialFlag_HasBaseColorTexture)
    {
        baseColor *= gBaseColorTexture.Sample(gLinearSampler, uv);
    }

    if (MaterialFlags & MaterialFlag_AlphaMask)
    {
        clip(baseColor.a - AlphaCutoff);
    }

    float metallic = PbrFactors.x;
    float roughness = PbrFactors.y;

    if (MaterialFlags & MaterialFlag_HasMetallicRoughnessTexture)
    {
        float4 mrSample = gMetallicRoughnessTexture.Sample(gLinearSampler, uv);

        roughness *= mrSample.g;
        metallic *= mrSample.b;
    }

    roughness = clamp(roughness, 0.04f, 1.0f);
    metallic = saturate(metallic);

    float occlusion = 1.0f;

    if (MaterialFlags & MaterialFlag_HasOcclusionTexture)
    {
        float aoSample = gOcclusionTexture.Sample(gLinearSampler, uv).r;
        occlusion = lerp(1.0f, aoSample, PbrFactors.w);
    }

    float3 emissive = EmissiveAndHeight.xyz;

    if (MaterialFlags & MaterialFlag_HasEmissiveTexture)
    {
        emissive *= gEmissiveTexture.Sample(gLinearSampler, uv).rgb;
    }

    float3 normal = input.normalWS;

    if (MaterialFlags & MaterialFlag_HasNormalTexture)
    {
        normal = ApplyNormalMap(TBN, uv);
    }
    else
    {
        normal = normalize(input.normalWS);
    }

    float3 ambient = baseColor.rgb * 0.015f * occlusion;
    float3 directionalLighting = AccumulateDirectionalLights(input.worldPosition, normal, viewDir, baseColor, metallic, roughness);
    float3 pointLighting = AccumulatePointLights(input.worldPosition, normal, viewDir, baseColor, metallic, roughness);

    float3 finalColor = ambient + directionalLighting + pointLighting + emissive;

    return float4(finalColor, baseColor.a);
}