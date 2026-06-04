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