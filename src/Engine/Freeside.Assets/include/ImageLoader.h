#pragma once

#include "Format.h"
#include "MaterialTypes.h"

#include <vector>
#include <string>

namespace Freeside
{
    class ImageLoader
    {
    public:
        ImageLoader();
        TextureDesc LoadImageWithWIC(const wchar_t* filePath);
        TextureDesc LoadHeightMapWithWIC(const wchar_t* filePath);
        TextureDesc CreateSolidColorImage(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    private:
    };
}