#pragma once

/**
 * @brief 对D3D12_ROOT_PARAMETER结构体的一些初始化。
 *        D3D12_ROOT_PARAMETER用来描述HLSL中的一个变量使用的寄存器。
 *        例如：
 *        @code
 *        Texture2D g_texture : register(t0);
 *        SamplerState g_sampler : register(s0);
 *        @endcode
 *        这里的t0就需要用D3D12_ROOT_PARAMETER来表示。而s0是通过D3D12_STATIC_SAMPLER_DESC描述的。
 *		  D3D12_ROOT_PARAMETER_TYPE里共有5种类型。
 */
class root_parameter
{
public:
	root_parameter();
	~root_parameter();

	void clear();

	/**
	 * @brief 初始化作为常量参数声明
	 * @param shader_register 例如HLSL中的“t0”寄存器，这里就传0
	 * @param num_dwords
	 * @param shader_visibility 哪个着色器可见
	 * @param register_space
	 */
	void init_as_constants(UINT shader_register, UINT num_dwords, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);

	/**
	 * @brief 初始化作为CBV参数声明
	 * @param shader_register
	 * @param shader_visibility
	 * @param register_space
	 */
	void init_as_buffer_cbv(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	
	/**
	 * @brief 初始化作为SRV参数声明
	 * @param shader_register
	 * @param shader_visibility
	 * @param register_space
	 */
	void init_as_buffer_srv(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	
	/**
	 * @brief 初始化作为UAV参数声明
	 * @param shader_register
	 * @param shader_visibility
	 * @param register_space
	 */
	void init_as_buffer_uav(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);
	
	/**
	 * @brief 初始化作为DescriptorTable，它由多个D3D12_DESCRIPTOR_RANGE组成，此时只分配内存但还未初始化，需要调用后面的set_table_range初始化每个RANGE项
	 * @param num_descriptor_ranges
	 * @param shader_visibility
	 */
	void init_as_descriptor_table(UINT num_descriptor_ranges, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
	
	/**
	 * @brief 设置DescriptorTable中的某个D3D12_DESCRIPTOR_RANGE项
	 * @param index
	 * @param range_type
	 * @param base_shader_register
	 * @param num_descriptors
	 * @param register_space
	 */
	void set_descriptor_range(UINT index, D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT num_descriptors, UINT register_space = 0);
	
	/**
	 * @brief 该接口是对上面两个接口的封装，相当于调用：
	 *		  @code
	 *		  init_as_descriptor_table(1, shader_visibility);
	 *		  set_table_range(0, range_type, base_shader_register, num_descriptors, register_space);
	 *		  @endcode
	 */
	void init_as_descriptor_range(D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT num_descriptors, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL, UINT register_space = 0);

private:
	D3D12_ROOT_PARAMETER rhi_root_parameter_;
};
