float SampleDepth(float2 uv)
{
    // white/high = shallow, black/low = deep
    return 1.0f - gHeightTexture.Sample(gLinearSampler, uv).r;
}

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

    float2 parallaxDir = viewDirTS.xy / max(abs(viewDirTS.z), 0.25f);
    parallaxDir = clamp(parallaxDir, -2.0f.xx, 2.0f.xx);

    float2 deltaUV = parallaxDir * heightScale / numLayers;

    float2 currentUV = uv;
    float2 previousUV = uv;

    float currentDepth = SampleDepth(currentUV);

    [loop]
    while (currentLayerDepth < currentDepth)
    {
        previousUV = currentUV;

        currentUV -= deltaUV;
        currentLayerDepth += layerDepth;

        currentDepth = SampleDepth(currentUV);
    }

    float afterDepth = currentDepth - currentLayerDepth;
    float beforeDepth = SampleDepth(previousUV) - (currentLayerDepth - layerDepth);

    float weight = afterDepth / (afterDepth - beforeDepth);
    weight = saturate(weight);

    return lerp(currentUV, previousUV, weight);
}