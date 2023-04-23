#pragma once

/*
===============================================================================
对D3D12_ROOT_PARAMETER结构体的一些初始化
===============================================================================
*/
class root_parameter
{
public:
	root_parameter();
	~root_parameter();

	void clear();
	void init_as_constants(UINT shader_register, UINT num_dwords, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	void init_as_constant_buffer(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	void init_as_buffer_srv(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	void init_as_buffer_uav(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	void init_as_descriptor_table(UINT num_descriptor_ranges, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
	void set_table_range(UINT range_index, D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT count, UINT register_space = 0);
	void init_as_descriptor_range(D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT count, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);

private:
	D3D12_ROOT_PARAMETER rhi_root_parameter_;
};
