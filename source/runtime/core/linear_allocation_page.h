#pragma once

#include "gpu_resource.h"

/**
 * @brief 表示GPU上的一块内存页
 * 
 * TODO:
 */
class linear_allocation_page : public gpu_resource
{
public:
    linear_allocation_page(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Usage);

    void unmap();

public:
};
