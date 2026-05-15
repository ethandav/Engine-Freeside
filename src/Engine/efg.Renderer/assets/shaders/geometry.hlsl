#define MAX_POINT_LIGHTS 1024

cbuffer CameraCB : register(b0)
{
    float4 ViewPosition;
    float4x4 ViewProjection;
};

cbuffer ShadowCB : register(b1)
{
    float4x4 LightViewProjection;
    float4 ShadowParams; // x = bias, y = strength, etc.
};

cbuffer DirectionalLightCB : register(b2)
{
    float4 LightDirectionAndIntensity;
    float4 LightColor;
};

cbuffer MaterialCB : register(b3)
{
    float4 BaseColor;
    float4 Specular;
    float2 uvScale;
}

struct PointLight
{
    float4 positionAndRadius; // xyz = position, w = radius
    float4 colorAndIntensity; // rgb = color, w = intensity
};

struct InstanceData
{
    float4x4 World;
};

StructuredBuffer<PointLight> gPointLights : register(t0);
StructuredBuffer<InstanceData> Instances : register(t1);
Texture2D gBaseColorTexture : register(t2);
Texture2D<float> ShadowMap : register(t3);
SamplerState gLinearSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

cbuffer PointLightMetadata : register(b4)
{
    uint gPointLightCount;
    float3 _pointLightPadding;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

VSOutput VSMain(VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput output;
    InstanceData instance = Instances[instanceId];

    float4 worldPosition = mul(instance.World, float4(input.position, 1.0f));
    
    output.position = mul(ViewProjection, worldPosition);
    output.worldPosition = worldPosition.xyz;
    output.normalWS = normalize(mul((float3x3) instance.World, input.normal));
    output.uv = input.uv;
    
    return output;
}

float ComputeShadowFactor(float3 worldPosition)
{
    float bias = ShadowParams.x;
    float strength = ShadowParams.y;

    float4 lightClip = mul(LightViewProjection, float4(worldPosition, 1.0f));

    // If using orthographic projection, w should usually be 1, but keep this for safety.
    float3 lightNdc = lightClip.xyz / lightClip.w;

    // Convert NDC [-1, 1] to texture UV [0, 1].
    float2 shadowUV;
    shadowUV.x = lightNdc.x * 0.5f + 0.5f;
    shadowUV.y = -lightNdc.y * 0.5f + 0.5f;

    float currentDepth = lightNdc.z;

    // Outside the shadow map: treat as lit.
    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f || shadowUV.y > 1.0f ||
        currentDepth < 0.0f || currentDepth > 1.0f)
    {
        return 1.0f;
    }

    float visibility = ShadowMap.SampleCmpLevelZero(
        ShadowSampler,
        shadowUV,
        currentDepth - bias
    );

    // visibility is 1 when lit, 0 when shadowed.
    return lerp(1.0f, visibility, strength);
}

float3 AccumulatePointLights(float3 worldPos, float3 normal, float3 viewDir, float4 baseColor)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    [loop]
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
        float ndotl = saturate(dot(normal, lightDir));
        float3 diffuse = baseColor.rgb * lightColor * ndotl * intensity * attenuation;
        float3 halfVector = normalize(lightDir + viewDir);
        float specularAmount = pow(saturate(dot(normal, halfVector)), Specular.y);
        float3 specular = lightColor * specularAmount * Specular.x * intensity * attenuation;

        result += diffuse + specular;
    }

    return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * uvScale;
    float4 sampledBaseColor = gBaseColorTexture.Sample(gLinearSampler, uv);

    float3 normal = normalize(input.normalWS);
    float3 viewDir = normalize(ViewPosition.xyz - input.worldPosition);

    float3 lightDir = normalize(-LightDirectionAndIntensity.xyz);
    float ndotl = saturate(dot(normal, lightDir));

    float shadow = ComputeShadowFactor(input.worldPosition);

    float3 ambient = sampledBaseColor.rgb * 0.1f;

    float3 diffuse =
        sampledBaseColor.rgb *
        LightColor.rgb *
        ndotl *
        LightDirectionAndIntensity.w;

    float3 halfVector = normalize(lightDir + viewDir);

    float specularAmount =
        pow(saturate(dot(normal, halfVector)), Specular.y);

    float3 specular =
        LightColor.rgb *
        specularAmount *
        Specular.x *
        LightDirectionAndIntensity.w;

    float3 pointLights =
        AccumulatePointLights(input.worldPosition, normal, viewDir, sampledBaseColor);

    float3 directionalLighting = shadow * (diffuse + specular);

    float3 finalColor =
        ambient +
        directionalLighting +
        pointLights;

    return float4(finalColor, BaseColor.a);
}