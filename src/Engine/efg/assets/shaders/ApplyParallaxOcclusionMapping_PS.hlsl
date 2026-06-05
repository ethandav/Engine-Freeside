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

    float2 deltaUV = (viewDirTS.xy / max(abs(viewDirTS.z), 0.1f)) * heightScale / numLayers;

    float2 currentUV = uv;

    // Convert height map to depth map:
    // white/high = shallow, black/low = deep.
    float currentDepthMapValue = 1.0f - gHeightTexture.Sample(gLinearSampler, currentUV).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentUV -= deltaUV;
        currentDepthMapValue = 1.0f - gHeightTexture.Sample(gLinearSampler, currentUV).r;
        currentLayerDepth += layerDepth;
    }

    return currentUV;
}