#include "gtest/gtest.h"
#include "..\efg.Renderer\include\d3d12\D3D12Context.h"
#include "..\efg.Renderer\include\d3d12\D3D12DescriptorContext.h"


class D3D12DescriptorTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ctx.Initialize(false);
        descriptors.Initialize(ctx.GetDevice());
    }

    void TearDown() override
    {
        // Usually nothing needed.
        // ComPtr members clean themselves up.
    }

    D3D12Context ctx = {};
    D3D12DescriptorContext descriptors = {};
};

namespace
{
    static constexpr UINT64 TestBufferSize = 1024;

    ComPtr<ID3D12Resource> CreateTestBuffer(
        ID3D12Device* device,
        UINT64 sizeInBytes,
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
    {
        ComPtr<ID3D12Resource> resource;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Alignment = 0;
        desc.Width = sizeInBytes;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = flags;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf()));

        EXPECT_TRUE(SUCCEEDED(hr));

        return resource;
    }

    ComPtr<ID3D12Resource> CreateTestRenderTarget(
        ID3D12Device* device)
    {
        ComPtr<ID3D12Resource> resource;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = 64;
        desc.Height = 64;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(resource.GetAddressOf()));

        EXPECT_TRUE(SUCCEEDED(hr));

        return resource;
    }

    ComPtr<ID3D12Resource> CreateTestDepthStencil(
        ID3D12Device* device)
    {
        ComPtr<ID3D12Resource> resource;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = 64;
        desc.Height = 64;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_D32_FLOAT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue,
            IID_PPV_ARGS(resource.GetAddressOf()));

        EXPECT_TRUE(SUCCEEDED(hr));

        return resource;
    }

    ComPtr<ID3D12Resource> CreateTestTexture2D(
        ID3D12Device* device)
    {
        ComPtr<ID3D12Resource> resource;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = 64;
        desc.Height = 64;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(resource.GetAddressOf()));

        EXPECT_TRUE(SUCCEEDED(hr));

        return resource;
    }

    D3D12DescriptorContext CreateDescriptorContext(
        D3D12Context& ctx)
    {
        D3D12DescriptorContext descriptorContext = {};
        descriptorContext.Initialize(ctx.GetDevice());
        return descriptorContext;
    }
}

TEST_F(D3D12DescriptorTests, CreateValidRTV)
{

    descriptors.CreateRTVDescriptorHeap(1);

    ComPtr<ID3D12Resource> resource =
        CreateTestRenderTarget(ctx.GetDevice());

    DescriptorAllocation allocation =
        descriptors.CreateRTV(resource.Get(), nullptr);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_EQ(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, RTVHeapThrowsWhenFull)
{
    descriptors.CreateRTVDescriptorHeap(1);

    ComPtr<ID3D12Resource> resource =
        CreateTestRenderTarget(ctx.GetDevice());

    descriptors.CreateRTV(resource.Get(), nullptr);

    EXPECT_THROW(
        descriptors.CreateRTV(resource.Get(), nullptr),
        std::runtime_error);
}

TEST_F(D3D12DescriptorTests, CreateValidDSV)
{
    descriptors.CreateDSVDescriptorHeap(1);

    ComPtr<ID3D12Resource> resource =
        CreateTestDepthStencil(ctx.GetDevice());

    DescriptorAllocation allocation =
        descriptors.CreateDSV(resource.Get(), nullptr);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_EQ(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, DSVHeapThrowsWhenFull)
{
    descriptors.CreateDSVDescriptorHeap(1);

    ComPtr<ID3D12Resource> resource =
        CreateTestDepthStencil(ctx.GetDevice());

    descriptors.CreateDSV(resource.Get(), nullptr);

    EXPECT_THROW(
        descriptors.CreateDSV(resource.Get(), nullptr),
        std::runtime_error);
}

TEST_F(D3D12DescriptorTests, CreateValidCBV)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(1);

    ComPtr<ID3D12Resource> buffer =
        CreateTestBuffer(ctx.GetDevice(), TestBufferSize);

    DescriptorAllocation allocation =
        descriptors.CreateCBV(
            buffer.Get(),
            64);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_NE(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, CreateValidStructuredBufferSRV)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(1);

    struct TestElement
    {
        float x, y, z, w;
    };

    constexpr uint32_t elementCount = 16;
    constexpr uint32_t elementStride = sizeof(TestElement);

    ComPtr<ID3D12Resource> buffer =
        CreateTestBuffer(
            ctx.GetDevice(),
            elementCount * elementStride);

    DescriptorAllocation allocation =
        descriptors.CreateStructuredBufferSRV(
            buffer.Get(),
            elementCount,
            elementStride);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_NE(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, CreateValidTexture2DSRV)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(1);

    ComPtr<ID3D12Resource> texture =
        CreateTestTexture2D(ctx.GetDevice());

    DescriptorAllocation allocation =
        descriptors.CreateTexture2DSRV(
            texture.Get(),
            DXGI_FORMAT_R8G8B8A8_UNORM,
            1);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_NE(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, CreateValidStructuredBufferUAV)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(1);

    struct TestElement
    {
        float x, y, z, w;
    };

    constexpr uint32_t elementCount = 16;
    constexpr uint32_t elementStride = sizeof(TestElement);

    ComPtr<ID3D12Resource> buffer =
        CreateTestBuffer(
            ctx.GetDevice(),
            elementCount * elementStride,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    DescriptorAllocation allocation =
        descriptors.CreateUAV(
            buffer.Get(),
            elementCount,
            elementStride,
            nullptr);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_NE(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, CBVSRVUAVHeapThrowsWhenFull)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(1);

    ComPtr<ID3D12Resource> buffer =
        CreateTestBuffer(ctx.GetDevice(), TestBufferSize);

    descriptors.CreateCBV(
        buffer.Get(),
        64);

    EXPECT_THROW(
        descriptors.CreateCBV(buffer.Get(), 64),
        std::runtime_error);
}

TEST_F(D3D12DescriptorTests, CreateValidSampler)
{
    descriptors.CreateSamplerDescriptorHeap(1);

    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    DescriptorAllocation allocation =
        descriptors.CreateSampler(samplerDesc);

    EXPECT_TRUE(allocation.IsValid());
    EXPECT_NE(allocation.cpu.ptr, 0);
    EXPECT_NE(allocation.gpu.ptr, 0);
}

TEST_F(D3D12DescriptorTests, SamplerHeapThrowsWhenFull)
{
    descriptors.CreateSamplerDescriptorHeap(1);

    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    descriptors.CreateSampler(samplerDesc);

    EXPECT_THROW(
        descriptors.CreateSampler(samplerDesc),
        std::runtime_error);
}

TEST_F(D3D12DescriptorTests, ShaderVisibleHeapGetterReturnsHeap)
{
    descriptors.CreateCBVSRVUAVDescriptorHeap(4);

    EXPECT_NE(descriptors.GetCBVSRVUAVHeap(), nullptr);
}

TEST_F(D3D12DescriptorTests, SamplerHeapGetterReturnsHeap)
{
    descriptors.CreateSamplerDescriptorHeap(4);

    EXPECT_NE(descriptors.GetSamplerHeap(), nullptr);
}