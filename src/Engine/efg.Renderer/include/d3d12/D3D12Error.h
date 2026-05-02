#pragma once

#include <stdexcept>
#include <string>
#include <Windows.h>

#define D3D12_THROW_IF_FAILED(x)                                      \
{                                                                  \
    HRESULT hr__ = (x);                                             \
    if (FAILED(hr__))                                               \
    {                                                              \
        throw std::runtime_error("D3D12 call failed: " #x);          \
    }                                                              \
}