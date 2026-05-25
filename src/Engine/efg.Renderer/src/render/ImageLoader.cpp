#include "..\..\include\render\ImageLoader.h"
#include "..\..\include\render\types\FormatUtils.h"
#pragma comment(lib, "windowscodecs.lib")

#include <wincodec.h>
#include <wrl.h>

namespace efg
{
    ImageLoader::ImageLoader()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    DecodedImage ImageLoader::LoadImageWithWIC(const wchar_t* filePath)
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

        DecodedImage image = {};
        image.width = width;
        image.height = height;
        image.format = Format::R8G8B8A8_UNorm;
        image.rowPitch = image.width * BytesPerPixelOrBlock(image.format);
        image.pixels.resize(static_cast<size_t>(image.rowPitch) * height);

        converter->CopyPixels(nullptr, image.rowPitch, static_cast<UINT>(image.pixels.size()), image.pixels.data());

        return image;
    }

    DecodedImage ImageLoader::CreateSolidColorImage(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        DecodedImage image = {};
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