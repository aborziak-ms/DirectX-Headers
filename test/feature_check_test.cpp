// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _WIN32
#include <wsl/winadapter.h>
#endif

#if ((__cplusplus >= 201703L) || _MSVC_LANG >= 201703L)
#define FEATURE_SUPPORT_USE_VARIANT
#endif
#include <directx/d3d12.h>
#include <directx/dxcore.h>
#include <directx/d3dx12.h>
#include "dxguids/dxguids.h"

int main()
{
    IDXCoreAdapter *adapter = nullptr;
    ID3D12Device *device = nullptr;

    {
        IDXCoreAdapterFactory *factory = nullptr;
        if (FAILED(DXCoreCreateAdapterFactory(&factory)))
            return -1;

        IDXCoreAdapterList *list = nullptr;
        if (FAILED(factory->CreateAdapterList(1, &DXCORE_ADAPTER_ATTRIBUTE_D3D12_CORE_COMPUTE, &list)))
            return -1;
        
        if (FAILED(list->GetAdapter(0, &adapter)))
            return -1;
    }

    if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
        return -1;
    }

#ifdef FEATURE_SUPPORT_USE_VARIANT
     CD3DX12FeatureSupport* features = nullptr;
     auto features_requested = CD3DX12FeatureSupport::Create(device);
     if (auto failure = std::get_if<HRESULT>(&features_requested)) {
         return -1;
     }

     features = std::get_if<CD3DX12FeatureSupport>(&features_requested);
     if (!features) {
         return -1;
     }
    // CD3DX12FeatureSupport* features = new CD3DX12FeatureSupport(device);
    if (FAILED(features->GetStatus())) {
        return -1;
    }
#else
    CD3DX12FeatureSupport feature_struct = CD3DX12FeatureSupport::Create(device);
    if (FAILED(feature_struct.GetStatus())) {
        return -1;
    }
    CD3DX12FeatureSupport* features = &feature_struct;
#endif

    // 0: D3D12_OPTIONS
    BOOL DoublePrecisionFloatShaderOps = features->DoublePrecisionFloatShaderOps();
    D3D12_CONSERVATIVE_RASTERIZATION_TIER ConservativeRasterizationTier = features->ConservativeRasterizationTier();

    // 2: Feature Levels
    D3D_FEATURE_LEVEL HighestLevelSupported = features->HighestFeatureLevel();

    // 3: Format Support
    D3D12_FORMAT_SUPPORT1 Support1;
    D3D12_FORMAT_SUPPORT2 Support2;
    if (FAILED(features->FormatSupport(DXGI_FORMAT_R16G16B16A16_TYPELESS, Support1, Support2))) {
        return -1;
    }

    // 4: Multisample Quality Support
    UINT NumQualityLevels;
    if (FAILED(features->MultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_TYPELESS, 1, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, NumQualityLevels))) {
        return -1;
    }

    // 5: Format Info
    UINT8 PlaneCount;
    if (FAILED(features->FormatInfo(DXGI_FORMAT_R16G16B16A16_TYPELESS, PlaneCount))) {
        return -1;
    }

    // 6: GPU Virtual Address Support
    UINT MaxGPUVABitsPerProcess = features->MaxGPUVirtualAddressBitsPerProcess();
    UINT MaxGPUVABitsPerResource = features->MaxGPUVirtualAddressBitsPerResource();

    // 7: Shader Model
    D3D_SHADER_MODEL HighestShaderModel = features->HighestShaderModel();
    
    // 8: Options1
    BOOL WaveOps = features->WaveOps();
    UINT WaveLaneCountMin = features->WaveLaneCountMin();
    UINT WaveLaneCountMax = features->WaveLaneCountMax();

    // 10: Protected Resource Session Support
    D3D12_PROTECTED_RESOURCE_SESSION_SUPPORT_FLAGS ProtectedResourceSessionSupport = features->ProtectedResourceSessionSupport();

    // 12: Root Signature
    D3D_ROOT_SIGNATURE_VERSION HighestRootSignatureVersion = features->HighestRootSignatureVersion();

    // 16: Architecture1
    BOOL TileBasedRenderer = features->TileBasedRenderer();
    BOOL UMA = features->UMA();
    BOOL CacheCoherentUMA = features->CacheCoherentUMA();
    BOOL IsolatedMMU = features->IsolatedMMU();

    return 0;
}
