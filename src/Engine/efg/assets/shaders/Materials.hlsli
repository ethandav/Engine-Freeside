Texture2D gBaseColorTexture : register(t5);
Texture2D gNormalTexture : register(t6);
Texture2D gMetallicRoughnessTexture : register(t7);
Texture2D gOcclusionTexture : register(t8);
Texture2D gEmissiveTexture : register(t9);
Texture2D gHeightTexture : register(t10);

static const uint MaterialFlag_HasBaseColorTexture = 1 << 0;
static const uint MaterialFlag_HasNormalTexture = 1 << 1;
static const uint MaterialFlag_HasMetallicRoughnessTexture = 1 << 2;
static const uint MaterialFlag_HasOcclusionTexture = 1 << 3;
static const uint MaterialFlag_HasEmissiveTexture = 1 << 4;
static const uint MaterialFlag_HasHeightTexture = 1 << 5;
static const uint MaterialFlag_AlphaMask = 1 << 6;
static const uint MaterialFlag_DoubleSided = 1 << 7;

cbuffer MaterialCB : register(b3)
{
    float4 BaseColorFactor; // rgba

    float4 PbrFactors;
    // x = metallicFactor
    // y = roughnessFactor
    // z = normalScale
    // w = occlusionStrength

    float4 EmissiveAndHeight;
    // xyz = emissiveFactor
    // w   = heightScale

    float4 UvTransform;
    // xy = uvScale
    // zw = uvOffset

    float4 SpecularCompat;
    // x = specularStrength
    // y = shininess
    // z/w unused

    uint MaterialFlags;
    float AlphaCutoff;
    float _pad0;
    float _pad1;
};