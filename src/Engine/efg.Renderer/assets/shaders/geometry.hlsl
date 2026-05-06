cbuffer CameraCB : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 World;
};

cbuffer DirectionalLightCB : register(b2)
{
    float4 LightDirectionAndIntensity;
    float4 LightColor;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normalWS : NORMAL;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    float4 worldPosition = mul(World, float4(input.position, 1.0f));
    output.position = mul(ViewProjection, worldPosition);
    output.normalWS = normalize(mul((float3x3) World, input.normal));
    
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 0.0f);
    float3 normal = normalize(input.normalWS);
    float3 lightDir = normalize(-LightDirectionAndIntensity.xyz);
    float intensity = LightDirectionAndIntensity.w;
    float ndotl = saturate(dot(normal, lightDir));
    float3 diffuse = color.rgb * LightColor.rgb * ndotl * intensity;
    float3 ambient = color.rgb * 0.1f;

    return float4(ambient + diffuse, color.a);
}