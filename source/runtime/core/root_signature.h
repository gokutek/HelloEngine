#pragma once

#include "pch.h"
#include "utility.h"
#include <memory>
#include <unordered_map>
#include <stdint.h>

class root_parameter;

/**
 * @brief root_signature与一个特定的HLSL强关联。
 * 
 * root_signature主要含ROOT_PARAMETER和STATIC_SAMPLER两部分。
 * 
 * 参考：
 * 
 * HLSL中声明RootSignature：https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
 */
class root_signature
{
public:
	/**
	 * @brief 构造函数
	 * @param root_params_num 即HLSL中的寄存器数量
	 * @param static_samplers_num 即HLSL中的寄存器数量(仅"s0"之类的)
	 */
	root_signature(uint32_t root_params_num, uint32_t static_samplers_num);

	~root_signature();

	/**
	 * @brief
	 */
	void destroy_all();

	/**
	 * @brief
	 */
	root_parameter& get_root_parameter(size_t index);

	/**
	 * @brief
	 */
	void init_static_sampler(uint32_t register_id, D3D12_SAMPLER_DESC const& desc, D3D12_SHADER_VISIBILITY visibility);

	/**
	 * @brief 序列化并创建ID3D12RootSignature
	 */
	void finalize(wchar_t const* name, D3D12_ROOT_SIGNATURE_FLAGS flags);

	/**
	 * @brief
	 */
	ID3D12RootSignature* get_signature() const;

private:
	/**
	 * @brief 重置
	 * @param root_params_num ROOT_PARAMETER参数数量
	 * @param static_samplers_num STATIC_SAMPLER参数数量
	 */
	void reset(uint32_t root_params_num, uint32_t static_samplers_num);

private:
	static std::unordered_map<size_t, ComPtr<ID3D12RootSignature> > rhi_root_signatures_map_;

	uint32_t parameters_num_; ///< 数组长度
	std::unique_ptr<root_parameter[]> param_array_;
	
	uint32_t static_samplers_num_; ///< 数组长度
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> static_sampler_array_;
	uint32_t init_static_samplers_num_; ///< 已用数量

	uint32_t descriptor_table_bmp_;
	uint32_t sampler_table_bmp_;
	uint32_t descriptor_table_size_[16];
	
	ComPtr<ID3D12RootSignature> rhi_root_signature_;
	bool finalized_;
};

inline ID3D12RootSignature* root_signature::get_signature() const
{ 
	return rhi_root_signature_.Get(); 
}
