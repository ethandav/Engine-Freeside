struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float3 tangentWS : TEXCOORD3;
};

cbuffer CameraCB : register(b0)
{
    float4 ViewPosition;
    float4x4 ViewProjection;
};

static const uint MaterialFlag_HasBaseColorTexture = 1 << 0;
static const uint MaterialFlag_HasNormalTexture = 1 << 1;
static const uint MaterialFlag_HasMetallicRoughnessTexture = 1 << 2;
static const uint MaterialFlag_HasOcclusionTexture = 1 << 3;
static const uint MaterialFlag_HasEmissiveTexture = 1 << 4;
static const uint MaterialFlag_HasHeightTexture = 1 << 5;
static const uint MaterialFlag_AlphaMask = 1 << 6;
static const uint MaterialFlag_DoubleSided = 1 << 7;

cbuffer MaterialCB : register(b3)
{
    float4 BaseColorFactor; // rgba

    float4 PbrFactors;
    // x = metallicFactor
    // y = roughnessFactor
    // z = normalScale
    // w = occlusionStrength

    float4 EmissiveAndHeight;
    // xyz = emissiveFactor
    // w   = heightScale

    float4 UvTransform;
    // xy = uvScale
    // zw = uvOffset

    float4 SpecularCompat;
    // x = specularStrength
    // y = shininess
    // z/w unused

    uint MaterialFlags;
    float AlphaCutoff;
    float _pad0;
    float _pad1;
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
Texture2D gNormalTexture : register(t3);
Texture2D gHeightTexture : register(t4);
StructuredBuffer<DirectionalLight> gDirectionalLights : register(t5);
StructuredBuffer<DirectionalShadowData> gDirectionalShadows : register(t6);
StructuredBuffer<PointShadowData> gPointShadows : register(t7);
SamplerState gLinearSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

float2 ApplyParallaxOcclusionMapping(float2 uv, float3 viewDirTS)
{
    viewDirTS = normalize(viewDirTS);

    const int minLayers = 8;
    const int maxLayers = 32;

    float ndotv = saturate(abs(viewDirTS.z));
    int numLayers = (int) lerp(maxLayers, minLayers, ndotv);

    float heightScale = 0.015f;

    float layerDepth = 1.0f / numLayers;
    float currentLayerDepth = 0.0f;

    float2 deltaUV = (viewDirTS.xy / max(abs(viewDirTS.z), 0.1f)) * heightScale / numLayers;

    float2 currentUV = uv;

    // Convert height map to depth map:
    // white/high = shallow, black/low = deep.
    float currentDepthMapValue = 1.0f - gHeightTexture.Sample(gLinearSampler, currentUV).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentUV -= deltaUV;
        currentDepthMapValue = 1.0f - gHeightTexture.Sample(gLinearSampler, currentUV).r;
        currentLayerDepth += layerDepth;
    }

    return currentUV;
}

float3 ApplyNormalMap(float3 normalWS, float3 tangentWS, float2 uv)
{
    float3 N = normalize(normalWS);
    float3 T = normalize(tangentWS);

    // Re-orthogonalize tangent against normal.
    T = normalize(T - N * dot(N, T));

    // Build bitangent.
    float3 B = normalize(cross(N, T));

    // Sample normal map.
    float3 normalTS = gNormalTexture.Sample(gLinearSampler, uv).xyz;

    // Convert from [0, 1] to [-1, 1].
    normalTS = normalTS * 2.0f - 1.0f;

    // Optional but usually good.
    normalTS = normalize(normalTS);

    // Tangent-space to world-space.
    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(normalTS, TBN));
}

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
        float specularAmount = pow(saturate(dot(normal, halfVector)), SpecularCompat.y);
        float3 specular = lightColor * specularAmount * SpecularCompat.x * intensity * attenuation;

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
        float specularAmount = pow(saturate(dot(normal, halfVector)), SpecularCompat.y);
        float3 specular = light.lightColor.rgb * specularAmount * SpecularCompat.x * light.lightDirectionAndIntensity.w;

        result += shadowFactor * (diffuse + specular);
    }

    return result;
}

void BuildTBN(
    float3 normalWS,
    float3 tangentWS,
    out float3 T,
    out float3 B,
    out float3 N)
{
    N = normalize(normalWS);

    // Re-orthogonalize tangent against normal.
    T = normalize(tangentWS - N * dot(tangentWS, N));

    // Depending on your handedness, this may need to be cross(T, N) instead.
    B = normalize(cross(N, T));
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * UvTransform.xy + UvTransform.zw;
    float3 viewDir = normalize(ViewPosition.xyz - input.worldPosition);

    if (MaterialFlags & MaterialFlag_HasHeightTexture)
    {
        float3 T, B, N;
        BuildTBN(input.normalWS, input.tangentWS, T, B, N);
        float3x3 TBN = float3x3(T, B, N);
        float3 viewDirTS = mul(viewDir, transpose(TBN));
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

    // Important: use parallaxed uv here too.
    float3 normal = ApplyNormalMap(input.normalWS, input.tangentWS, uv);

    float3 ambient = baseColor.rgb * 0.1f;
    float3 directionalLighting = AccumulateDirectionalLights(input.worldPosition, normal, viewDir, baseColor);
    float3 pointLighting = AccumulatePointLights(input.worldPosition, normal, viewDir, baseColor);

    float3 finalColor = ambient + directionalLighting + pointLighting;

    return float4(finalColor, baseColor.a);
}