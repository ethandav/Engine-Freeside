Texture2D gSceneColor : register(t0);
SamplerState gLinearSampler : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : TEXCOORD0) : SV_TARGET
{
    float3 hdr = gSceneColor.Sample(gLinearSampler, uv).rgb;

    // Simple temporary Reinhard tonemap.
    float3 ldr = hdr / (hdr + 1.0f);

    // If backbuffer RTV is SRGB, do NOT gamma encode manually.
    return float4(ldr, 1.0f);
}