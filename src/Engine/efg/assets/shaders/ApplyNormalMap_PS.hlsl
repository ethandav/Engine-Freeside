float3 ApplyNormalMap(float3x3 TBN, float2 uv)
{
    // Sample normal map.
    float3 normalTS = gNormalTexture.Sample(gLinearSampler, uv).xyz;

    // Convert from [0, 1] to [-1, 1].
    normalTS = normalTS * 2.0f - 1.0f;

    // Optional but usually good.
    normalTS = normalize(normalTS);

    return normalize(mul(normalTS, TBN));
}