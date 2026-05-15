#pragma once

#include "types/Format.h"

#include <vector>
#include <string>

namespace efg
{
    struct DecodedImage
    {
        std::vector<uint8_t> pixels;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t rowPitch = 0;
        Format format = Format::Unknown;
    };

    class ImageLoader
    {
    public:
        ImageLoader();
        DecodedImage LoadImageWithWIC(const wchar_t* filePath);
    private:
    };
}