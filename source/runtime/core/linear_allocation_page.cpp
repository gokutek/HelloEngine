#include "linear_allocation_page.h"

linear_allocation_page::linear_allocation_page(ID3D12Resource* rhi_resource, D3D12_RESOURCE_STATES initial_resource_state)
{
	rhi_resource_.Attach(rhi_resource);
	usage_state_ = initial_resource_state;
	gpu_virtual_address_ = rhi_resource_->GetGPUVirtualAddress();
	rhi_resource_->Map(0, nullptr, &cpu_virtual_address_);
}

linear_allocation_page::~linear_allocation_page()
{
	unmap();
}

void linear_allocation_page::map()
{
	if (!cpu_virtual_address_)
	{
		rhi_resource_->Map(0, nullptr, &cpu_virtual_address_);
	}
}

void linear_allocation_page::unmap()
{
	if (cpu_virtual_address_)
	{
		rhi_resource_->Unmap(0, nullptr);
		cpu_virtual_address_ = nullptr;
	}
}
