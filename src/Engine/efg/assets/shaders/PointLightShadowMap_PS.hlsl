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

cbuffer FarPlaneCB : register(b1)
{
    float FarPlane;
};

float4 PSMain(VSOutput input) : SV_TARGET
{
    float distanceToLight = length(input.worldPosition - ViewPosition.xyz);
    return saturate(distanceToLight / FarPlane);
}