#include "..\..\include\render\ImageLoader.h"
#include "..\..\include\render\types\FormatUtils.h"
#pragma comment(lib, "windowscodecs.lib")

#include <wincodec.h>
#include <wrl.h>

namespace efg
{
    ImageLoader::ImageLoader()
    {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    DecodedImage ImageLoader::LoadImageWithWIC(const wchar_t* filePath)
    {
        using Microsoft::WRL::ComPtr;

        ComPtr<IWICImagingFactory> factory;
        CoCreateInstance(CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(factory.GetAddressOf())
        );

        ComPtr<IWICBitmapDecoder> decoder;
        factory->CreateDecoderFromFilename(
            filePath,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        );

        ComPtr<IWICBitmapFrameDecode> frame;
        decoder->GetFrame(0, frame.GetAddressOf());

        UINT width = 0;
        UINT height = 0;
        frame->GetSize(&width, &height);

        ComPtr<IWICFormatConverter> converter;
        factory->CreateFormatConverter(converter.GetAddressOf());

        converter->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom
        );

        DecodedImage image = {};
        image.width = width;
        image.height = height;
        image.format = Format::R8G8B8A8_UNorm;
        image.rowPitch = image.width * BytesPerPixelOrBlock(image.format);
        image.pixels.resize(static_cast<size_t>(image.rowPitch) * height);

        converter->CopyPixels(
            nullptr,
            image.rowPitch,
            static_cast<UINT>(image.pixels.size()),
            image.pixels.data()
        );

        return image;
    }
}