#pragma once

#include "pch.h"
#include <stdint.h>

/*
===============================================================================
===============================================================================
*/
class gpu_resource
{
public:
	gpu_resource();
	gpu_resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES current_state);
	virtual ~gpu_resource();

	virtual void destroy();
	ID3D12Resource* get_resource();
	const ID3D12Resource* get_resource() const;
	ID3D12Resource** get_address_of();
	D3D12_GPU_VIRTUAL_ADDRESS get_gpu_virtual_address() const;
	uint32_t get_version_id() const;
	ID3D12Resource* operator->();
	const ID3D12Resource* operator->() const;

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> rhi_resource_;
	D3D12_RESOURCE_STATES usage_state_;
	D3D12_RESOURCE_STATES transitioning_state_;
	D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_address_;
	uint32_t version_id_;
};

inline gpu_resource::gpu_resource() :
	usage_state_(D3D12_RESOURCE_STATE_COMMON),
	transitioning_state_(D3D12_RESOURCE_STATES(-1)),
	gpu_virtual_address_(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
	version_id_(0)
{
}

inline gpu_resource::gpu_resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES current_state) :
	rhi_resource_(resource),
	usage_state_(current_state),
	transitioning_state_(D3D12_RESOURCE_STATES(-1)),
	gpu_virtual_address_(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
	version_id_(0)
{
}

inline gpu_resource::~gpu_resource()
{
	destroy();
}

inline void gpu_resource::destroy()
{
	rhi_resource_ = nullptr;
	gpu_virtual_address_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
	++version_id_;
}

inline ID3D12Resource* gpu_resource::get_resource()
{
	return rhi_resource_.Get();
}

inline const ID3D12Resource* gpu_resource::get_resource() const
{
	return rhi_resource_.Get();
}

inline ID3D12Resource** gpu_resource::get_address_of()
{
	return rhi_resource_.GetAddressOf();
}

inline D3D12_GPU_VIRTUAL_ADDRESS gpu_resource::get_gpu_virtual_address() const
{
	return gpu_virtual_address_;
}

inline uint32_t gpu_resource::get_version_id() const
{
	return version_id_;
}

inline ID3D12Resource* gpu_resource::operator->()
{
	return get_resource();
}

inline const ID3D12Resource* gpu_resource::operator->() const
{
	return get_resource();
}
