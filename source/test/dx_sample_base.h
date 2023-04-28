#pragma once

#include <Windows.h>
#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <wrl.h>
#include <shellapi.h>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

/**
 * @brief 获取exe文件所在目录
 * @return exe所在目录名(例如"c:\test\")
 */
inline std::wstring get_asset_path()
{
    wchar_t path[MAX_PATH] = { 0 };
    DWORD size = GetModuleFileName(nullptr, path, MAX_PATH);
    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
    return path;
}

class SampleBase
{
public:
    SampleBase(UINT width, UINT height, std::wstring const& name) :
        window_width_(width),
        window_height_(height),
        assets_path_(get_asset_path()),
        window_title_(name)
    {
        aspect_ratio_ = 1.0f * width / height;
    }

    virtual ~SampleBase()
    {
    }

    UINT GetWidth() const { return window_width_; }
    UINT GetHeight() const { return window_height_; }
    wchar_t const* GetTitle() const { return window_title_.c_str(); }

    /**
     * @brief Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
     *        If no such adapter can be found, *ppAdapter will be set to nullptr.
     */
    void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) const
    {
        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter)));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }

    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnDestroy() = 0;

    virtual void OnKeyDown(UINT8 key) {}
    virtual void OnKeyUp(UINT8 key) {}

protected:
    UINT window_width_; ///< 窗口宽度
    UINT window_height_; ///< 窗口高度
    float aspect_ratio_; ///< 窗口宽高比
    std::wstring window_title_; ///< 窗口标题
    std::wstring assets_path_; ///< exe文件所在目录
};
