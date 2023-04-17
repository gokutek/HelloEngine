#include "graphics_core.h"
#include "util/command_line_arg.h"
#include "utility.h"

static const uint32_t VENDOR_ID_NVIDIA	= 0x10DE;
static const uint32_t VENDORID_AMD		= 0x1002;
static const uint32_t VENDORID_INTEL	= 0x8086;

static bool g_bTypedUAVLoadSupport_R11G11B10_FLOAT = false;
static bool g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

// 检查是否支持光线追踪?
static bool IsDirectXRaytracingSupported(ID3D12Device* testDevice)
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupport = {};

	if (FAILED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupport, sizeof(featureSupport))))
	{
		return false;
	}

	return featureSupport.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

int graphics::init_device(bool RequireDXRSupport)
{
	ComPtr<ID3D12Device> pDevice;

#if _DEBUG
	uint32_t useDebugLayers = 1;
#else
	uint32_t useDebugLayers = 0;
	CommandLineArgs::GetInteger(L"debug", useDebugLayers);
#endif

	DWORD dxgiFactoryFlags = 0;

	if (useDebugLayers)
	{
		ComPtr<ID3D12Debug> debugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->EnableDebugLayer();

			uint32_t useGPUBasedValidation = 0;
			CommandLineArgs::GetInteger(L"gpu_debug", useGPUBasedValidation);
			if (useGPUBasedValidation)
			{
				ComPtr<ID3D12Debug1> debugInterface1;
				if (SUCCEEDED((debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)))))
				{
					debugInterface1->SetEnableGPUBasedValidation(true);
				}
			}
		}
		else
		{
			utility::Print("WARNING:  Unable to enable D3D12 debug validation layer\n");
		}

#if _DEBUG
		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
			};
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
#endif
	}

	// Obtain the DXGI factory
	ComPtr<IDXGIFactory6> dxgiFactory;
	ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	// Create the D3D graphics device
	ComPtr<IDXGIAdapter1> pAdapter;

	uint32_t bUseWarpDriver = false;
	CommandLineArgs::GetInteger(L"warp", bUseWarpDriver);

	uint32_t desiredVendor = VENDOR_ID_NVIDIA;

	// Temporary workaround because SetStablePowerState() is crashing
	D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

	if (!bUseWarpDriver)
	{
		SIZE_T MaxSize = 0;

		for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx)
		{
			DXGI_ADAPTER_DESC1 desc;
			pAdapter->GetDesc1(&desc);

			// Is a software adapter?
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// Is this the desired vendor desired?
			if (desiredVendor != 0 && desiredVendor != desc.VendorId)
				continue;

			// Can create a D3D12 device?
			if (FAILED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
				continue;

			// Does support DXR if required?
			if (RequireDXRSupport && !IsDirectXRaytracingSupported(pDevice.Get()))
				continue;

			// By default, search for the adapter with the most memory because that's usually the dGPU.
			if (desc.DedicatedVideoMemory < MaxSize)
				continue;

			MaxSize = desc.DedicatedVideoMemory;

			if (graphics::device != nullptr)
				graphics::device->Release();

			graphics::device = pDevice.Detach();

			utility::Printf(L"Selected GPU:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
		}
	}

	if (RequireDXRSupport && !graphics::device)
	{
		utility::Printf("Unable to find a DXR-capable device. Halting.\n");
		__debugbreak();
	}

	if (graphics::device == nullptr)
	{
		if (bUseWarpDriver)
			utility::Print("WARP software adapter requested.  Initializing...\n");
		else
			utility::Print("Failed to find a hardware adapter.  Falling back to WARP.\n");
		ASSERT_SUCCEEDED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter)));
		ASSERT_SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)));
		graphics::device = pDevice.Detach();
	}
	else
	{
#if _DEBUG
		bool DeveloperModeEnabled = false;

		// Look in the Windows Registry to determine if Developer Mode is enabled
		HKEY hKey;
		LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
		if (result == ERROR_SUCCESS)
		{
			DWORD keyValue, keySize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
			if (result == ERROR_SUCCESS && keyValue == 1)
				DeveloperModeEnabled = true;
			RegCloseKey(hKey);
		}

		WARN_ONCE_IF_NOT(DeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results");

		// Prevent the GPU from overclocking or underclocking to get consistent timings
		if (DeveloperModeEnabled)
			graphics::device->SetStablePowerState(TRUE);
#endif
	}

	return 0;
}

int graphics::init_info_queue()
{
#if _DEBUG
	ID3D12InfoQueue* pInfoQueue = nullptr;
	if (SUCCEEDED(graphics::device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
	{
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] =
		{
			// This occurs when there are uninitialized descriptors in a descriptor table, even when a
			// shader does not access the missing descriptors.  I find this is common when switching
			// shader permutations and not wanting to change much code to reorder resources.
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

			// Triggered when a shader does not export all color components of a render target, such as
			// when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
			D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

			// This occurs when a descriptor table is unbound even when a shader does not access the missing
			// descriptors.  This is common with a root signature shared between disparate shaders that
			// don't all need the same types of resources.
			D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

			// RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
			(D3D12_MESSAGE_ID)1008,
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		pInfoQueue->PushStorageFilter(&NewFilter);
		pInfoQueue->Release();
	}
#endif

	return 0;
}

int graphics::check_feature_support()
{
	// We like to do read-modify-write operations on UAVs during post processing.  To support that, we
	// need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
	// decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
	// load support.
	D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
	if (SUCCEEDED(graphics::device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
	{
		if (FeatureData.TypedUAVLoadAdditionalFormats)
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
			{
				DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
			};

			if (SUCCEEDED(graphics::device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
				(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
			{
				g_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
			}

			Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			if (SUCCEEDED(graphics::device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
				(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
			{
				g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
			}
		}
	}

	return 0;
}

void graphics::initialize(bool RequireDXRSupport)
{
	D3D12_DESCRIPTOR_HEAP_TYPE heap_types[] = 
	{
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	};

	for (int i = 0; i < _countof(heap_types); ++i)
	{
		descriptor_allocators_.push_back(std::make_unique<descriptor_allocator>(heap_types[i]));
	}

	init_device(RequireDXRSupport);
	init_info_queue();
	check_feature_support();

	command_manager_.create(device);
	//TODO: init common state
	display_.initialize();
}

void graphics::shutdown()
{
	//TODO:
}

D3D12_CPU_DESCRIPTOR_HANDLE graphics::allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32_t count)
{
	return descriptor_allocators_[heap_type]->allocate(count);
}
