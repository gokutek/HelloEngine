#include "display.h"
#include "utility.h"
#include "graphics_core.h"
#include "global.h"

#define SWAP_CHAIN_BUFFER_COUNT 3
#define SWAP_CHAIN_FORMAT  DXGI_FORMAT_R10G10B10A2_UNORM

display::display() :
	display_width_(1280),
	display_height_(720),
	current_buffer_(0),
	rhi_swap_chain_(nullptr)
{
	std::unique_ptr<color_buffer[]> display_plane_;
}

void display::initialize()
{

	//创建factory
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = display_width_;
	desc.Height = display_height_;
	desc.Format = SWAP_CHAIN_FORMAT;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	desc.Scaling = DXGI_SCALING_NONE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc1 = {};
	desc1.Windowed = TRUE;

	ASSERT_SUCCEEDED(factory->CreateSwapChainForHwnd(get_rhi()->command_manager_.get_graphics_queue().get_command_queue(), g_hWnd, &desc, &desc1, nullptr, rhi_swap_chain_.GetAddressOf()));

	//TODO: 支持HDR输出?

	//初始化各个后台缓存
	display_plane_.reset(new color_buffer[SWAP_CHAIN_BUFFER_COUNT]);
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		rhi_swap_chain_->GetBuffer(i, IID_PPV_ARGS(&buffer));
		display_plane_[i].create_from_swap_chain(L"Primary SwapChain Buffer", buffer.Detach());
	}

	set_native_resolution();
}

void display::shutdown()
{
	//TODO:
}

void display::resize(uint32_t width, uint32_t height)
{
	//TODO:
}

void display::present()
{
	//TODO:
}

void display::set_native_resolution()
{
	//TODO:
}
