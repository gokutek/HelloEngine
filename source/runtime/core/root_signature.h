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
 * D3D12_ROOT_SIGNATURE_DESC含两部分：ROOT_PARAMETER和STATIC_SAMPLER。
 * 
 * 参考：
 * HLSL中声明RootSignature：https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
 */
class root_signature
{
public:
	/**
	 * @brief 构造函数
	 * @param root_params_num 即HLSL中声明的ROOT_PARAM个数
	 * @param static_samplers_num 即HLSL中声明的StaticSampler数量
	 * 
	 * 例如对于下面HLSL中的声明，有7个ROOT_PARAM、3个StaticSampler：
	 * @code
	 * #define Renderer_RootSig \
	 *		"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	 *		"CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
	 *		"CBV(b0, visibility = SHADER_VISIBILITY_PIXEL), " \
	 *		"DescriptorTable(SRV(t0, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
	 *		"DescriptorTable(Sampler(s0, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
	 *		"DescriptorTable(SRV(t10, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
	 *		"CBV(b1), " \
	 *		"SRV(t20, visibility = SHADER_VISIBILITY_VERTEX), " \
	 *		"StaticSampler(s10, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_PIXEL)," \
	 *		"StaticSampler(s11, visibility = SHADER_VISIBILITY_PIXEL," \
	 *		    "addressU = TEXTURE_ADDRESS_CLAMP," \
	 *		    "addressV = TEXTURE_ADDRESS_CLAMP," \
	 *		    "addressW = TEXTURE_ADDRESS_CLAMP," \
	 *		    "comparisonFunc = COMPARISON_GREATER_EQUAL," \
	 *		    "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT)," \
	 *		"StaticSampler(s12, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_PIXEL)"
	 * @endcode
	 */
	root_signature(uint32_t root_params_num, uint32_t static_samplers_num);

	~root_signature();

	/**
	 * @brief
	 */
	void destroy_all();

	/**
	 * @brief 获得指定索引的root_parameter，用于对它初始化
	 * @param index 索引
	 */
	root_parameter& get_root_parameter(size_t index);

	/**
	 * @brief
	 */
	void init_static_sampler(uint32_t register_id, D3D12_SAMPLER_DESC const& desc, D3D12_SHADER_VISIBILITY visibility);

	/**
	 * @brief 序列化并创建ID3D12RootSignature。调用本接口前确保已经初始化好了ROOT_PARAMETER和STATIC_SAMPLER。
	 */
	void finalize(wchar_t const* name, D3D12_ROOT_SIGNATURE_FLAGS flags);

	/**
	 * @brief 获取最终创建的RootSignature
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
