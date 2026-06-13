float3 DecodeNormalMap(float3 normalSample, float normalScale)
{
    // Texture is [0, 1], convert to [-1, 1]
    float3 n;
    n.xy = normalSample.xy * 2.0f - 1.0f;
    n.z = normalSample.z * 2.0f - 1.0f;

    // Scale tangent-space X/Y only
    n.xy *= normalScale;

    // Re-normalize after scaling
    return normalize(n);
}

float3 ApplyNormalMap(float3x3 TBN, float2 uv)
{
    float3 normalTS = DecodeNormalMap(gNormalTexture.Sample(gLinearSampler, uv).xyz, PbrFactors.z);
    
    return normalize(mul(normalTS, TBN));
}