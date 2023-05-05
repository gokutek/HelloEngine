#pragma once

#include "pch.h"
#include "utility.h"
#include <stdint.h>

/**
 * @brief 表示一个GPU资源（ID3D12Resource）
 * 
 * 该类本身不会被直接使用，而是使用它的子类，由子类中负责创建资源。
 */
class gpu_resource
{
public:
	gpu_resource();

	/**
	 * @brief
	 */
	gpu_resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES current_state);

	virtual ~gpu_resource();

	/**
	 * @brief
	 */
	virtual void destroy();

	/**
	 * @brief
	 */
	ID3D12Resource* get_resource();

	/**
	 * @brief 同上
	 */
	const ID3D12Resource* get_resource() const;

	/**
	 * @brief
	 */
	ID3D12Resource* operator->();

	/**
	 * @brief 同上
	 */
	const ID3D12Resource* operator->() const;

	/**
	 * @brief
	 */
	ID3D12Resource** get_address_of();
	
	/**
	 * @brief
	 */
	D3D12_GPU_VIRTUAL_ADDRESS get_gpu_virtual_address() const;

	/**
	 * @brief
	 */
	D3D12_RESOURCE_STATES get_usage_state() const;

	/**
	 * @brief
	 */
	void set_usage_state(D3D12_RESOURCE_STATES new_state);

	/**
	 * @brief
	 */
	D3D12_RESOURCE_STATES get_transitioning_state() const;

	/**
	 * @brief
	 */
	void set_transitioning_state(D3D12_RESOURCE_STATES new_state);

	/**
	 * @brief
	 */
	uint32_t get_version_id() const;

protected:
	ComPtr<ID3D12Resource> rhi_resource_;
	D3D12_RESOURCE_STATES usage_state_;
	D3D12_RESOURCE_STATES transitioning_state_;
	D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_address_; //TODO:这个为什么要放基类里
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

inline ID3D12Resource* gpu_resource::operator->()
{
	return get_resource();
}

inline const ID3D12Resource* gpu_resource::operator->() const
{
	return get_resource();
}

inline ID3D12Resource** gpu_resource::get_address_of()
{
	return rhi_resource_.GetAddressOf();
}

inline D3D12_GPU_VIRTUAL_ADDRESS gpu_resource::get_gpu_virtual_address() const
{
	return gpu_virtual_address_;
}

inline D3D12_RESOURCE_STATES gpu_resource::get_usage_state() const
{
	return usage_state_;
}

inline void gpu_resource::set_usage_state(D3D12_RESOURCE_STATES new_state)
{
	usage_state_ = new_state;
}

inline D3D12_RESOURCE_STATES gpu_resource::get_transitioning_state() const
{
	return transitioning_state_;
}

inline void gpu_resource::set_transitioning_state(D3D12_RESOURCE_STATES new_state)
{
	transitioning_state_ = new_state;
}

inline uint32_t gpu_resource::get_version_id() const
{
	return version_id_;
}
