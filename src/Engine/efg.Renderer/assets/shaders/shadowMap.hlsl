#define MAX_POINT_LIGHTS 1024

cbuffer LightViewCB : register(b0)
{
    float4x4 ViewProjection;
};

struct InstanceData
{
    float4x4 World;
};

StructuredBuffer<InstanceData> Instances : register(t0);

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