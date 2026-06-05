#include "..\include\ImageLoader.h"
#include "..\include\MaterialTypes.h"
#include "..\include\FormatUtils.h"
#pragma comment(lib, "windowscodecs.lib")

#include <wincodec.h>
#include <wrl.h>

namespace Freeside
{
    ImageLoader::ImageLoader()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    TextureDesc ImageLoader::LoadImageWithWIC(const wchar_t* filePath)
    {
        using Microsoft::WRL::ComPtr;

        ComPtr<IWICImagingFactory> factory;
        ComPtr<IWICBitmapDecoder> decoder;
        ComPtr<IWICBitmapFrameDecode> frame;
        ComPtr<IWICFormatConverter> converter;
        UINT width = 0;
        UINT height = 0;

        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
        hr = factory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());
        decoder->GetFrame(0, frame.GetAddressOf());
        frame->GetSize(&width, &height);
        factory->CreateFormatConverter(converter.GetAddressOf());
        converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

        TextureDesc image = {};
        image.width = width;
        image.height = height;
        image.format = Format::R8G8B8A8_UNorm;
        image.rowPitch = image.width * BytesPerPixelOrBlock(image.format);
        image.pixels.resize(static_cast<size_t>(image.rowPitch) * height);

        converter->CopyPixels(nullptr, image.rowPitch, static_cast<UINT>(image.pixels.size()), image.pixels.data());

        return image;
    }

    TextureDesc ImageLoader::LoadHeightMapWithWIC(const wchar_t* filePath)
    {
        TextureDesc rgba = LoadImageWithWIC(filePath);

        TextureDesc height = {};
        height.width = rgba.width;
        height.height = rgba.height;
        height.format = Format::R8_UNorm;
        height.rowPitch = height.width * BytesPerPixelOrBlock(height.format);
        height.pixels.resize(static_cast<size_t>(height.rowPitch) * height.height);

        for (uint32_t y = 0; y < height.height; ++y)
        {
            const uint8_t* srcRow = rgba.pixels.data() + static_cast<size_t>(y) * rgba.rowPitch;
            uint8_t* dstRow = height.pixels.data() + static_cast<size_t>(y) * height.rowPitch;

            for (uint32_t x = 0; x < height.width; ++x)
            {
                const uint8_t r = srcRow[x * 4 + 0];
                const uint8_t g = srcRow[x * 4 + 1];
                const uint8_t b = srcRow[x * 4 + 2];

                // Since your generated height map is grayscale, r/g/b should be similar.
                // You can use r only, or luminance. Luminance is safer.
                dstRow[x] = static_cast<uint8_t>(
                    0.299f * r + 0.587f * g + 0.114f * b
                    );
            }
        }

        return height;
    }

    TextureDesc ImageLoader::CreateSolidColorImage(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        TextureDesc image = {};
        image.width = 1;
        image.height = 1;
        image.format = Format::R8G8B8A8_UNorm;
        image.rowPitch = image.width * BytesPerPixelOrBlock(image.format);
        image.pixels.resize(image.rowPitch * image.height);
        image.pixels[0] = r;
        image.pixels[1] = g;
        image.pixels[2] = b;
        image.pixels[3] = a;

        return image;
    }
}