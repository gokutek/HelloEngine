#pragma once

#include "gpu_resource.h"

/**
 * @brief 抽象表示一块BUFF内存资源
 */
class linear_allocation_page : public gpu_resource
{
public:
    /**
     * @brief 构造函数
     * @param rhi_resource 由外部创建的资源，所有权将转移到本对象
     * @param initial_resource_state 初始状态
     */
    linear_allocation_page(ID3D12Resource* rhi_resource, D3D12_RESOURCE_STATES initial_resource_state);

    /**
     * @brief 析构时会自动unmap
     */
    ~linear_allocation_page();

    /**
     * @brief 映射到CPU
     */
    void map();

    /**
     * @brief 与map对应
     */
    void unmap();

public:
    D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_address_; ///< GPU内存地址
    void* cpu_virtual_address_; ///< 映射到CPU的内存地址
};
