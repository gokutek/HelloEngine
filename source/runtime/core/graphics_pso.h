#pragma once

#include "pch.h"
#include "pso.h"
#include <memory>
#include <string>

/**
 * @brief 
 * 
 * 各种set接口都是简单地设置D3D12_GRAPHICS_PIPELINE_STATE_DESC结构体成员变量；
 * finalize接口根据D3D12_GRAPHICS_PIPELINE_STATE_DESC创建ID3D12PipelineState对象；
 */
class graphics_pso : public pso
{
public:
	graphics_pso(wchar_t const* name);

    void set_blend_state(D3D12_BLEND_DESC const& blend_desc);
    void set_rasterizer_state(D3D12_RASTERIZER_DESC const& rasterizer_desc);
    void set_depth_stencil_state(D3D12_DEPTH_STENCIL_DESC const& depth_stencil_desc);
    void set_sample_mask(UINT sample_mask);
    void set_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type);
    void set_depth_target_format(DXGI_FORMAT dsv_format, UINT msaa_count = 1, UINT msaa_quality = 0);
    void set_render_target_format(DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format, UINT msaa_count = 1, UINT msaa_quality = 0);
    void set_render_target_formats(UINT num_rtvs, DXGI_FORMAT const* rtv_formats, DXGI_FORMAT dsv_format, UINT msaa_count = 1, UINT msaa_quality = 0);

    /**
     * @brief 
     * @param num_elements 
     * @param input_element_descs 这块内存会被拷贝，因此调用完后可以安全释放
     */
    void set_input_layout(UINT num_elements, D3D12_INPUT_ELEMENT_DESC const* input_element_descs);

    void set_primitive_restart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE value);

    void set_vertex_shader(void const* shader_bytecode, size_t size);
    void set_pixel_shader(void const* shader_bytecode, size_t size);
    void set_geometry_shader(void const* shader_bytecode, size_t size);
    void set_hull_shader(void const* shader_bytecode, size_t size);
    void set_domain_shader(void const* shader_bytecode, size_t size);

    void set_vertex_shader(D3D12_SHADER_BYTECODE const& shader_bytecode);
    void set_pixel_shader(D3D12_SHADER_BYTECODE const& shader_bytecode);
    void set_geometry_shader(D3D12_SHADER_BYTECODE const& shader_bytecode);
    void set_hull_shader(D3D12_SHADER_BYTECODE const& shader_bytecode);
    void set_domain_shader(D3D12_SHADER_BYTECODE const& shader_bytecode);

    int finalize();

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC state_desc_;
    std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> input_layouts_;
};
