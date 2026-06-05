struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normalWS : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tangentWS : TEXCOORD3;
};

cbuffer PointShadowCB : register(b0)
{
    float4 PositionAndFarPlane; // xyz = position, w = farPlane
};

float4 PSMain(VSOutput input) : SV_TARGET
{
    float distanceToLight = length(input.worldPosition - PositionAndFarPlane.xyz);
    return saturate(distanceToLight / PositionAndFarPlane.w);
}