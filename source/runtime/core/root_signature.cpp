#include "root_signature.h"
#include "graphics_core.h"

class root_parameter : public D3D12_ROOT_PARAMETER
{
public:
};

std::unordered_map<size_t, ComPtr<ID3D12RootSignature> > root_signature::rhi_root_signatures_map_;

root_signature::root_signature(uint32_t root_params_num, uint32_t static_samplers_num) :
	finalized_(false),
	parameters_num_(0),
	samplers_num_(0),
	init_static_samplers_num_(0),
	descriptor_table_bmp_(0),
	sampler_table_bmp_(0)
{
	memset(&descriptor_table_size_, 0, sizeof(descriptor_table_size_));

	reset(root_params_num, static_samplers_num);
}

root_signature::~root_signature()
{
	//TODO:
}

void root_signature::destroy_all()
{
	rhi_root_signatures_map_.clear();
}

void root_signature::reset(uint32_t root_params_num, uint32_t static_samplers_num)
{
	parameters_num_ = root_params_num;
	samplers_num_ = static_samplers_num;
	init_static_samplers_num_ = 0;
	param_array_.reset(new root_parameter[root_params_num]);
	sampler_array_.reset(new D3D12_STATIC_SAMPLER_DESC[static_samplers_num]);
}

D3D12_ROOT_PARAMETER& root_signature::get_root_parameter(size_t index)
{
	ASSERT(index < parameters_num_);
	return param_array_[index];
}

void root_signature::init_static_sampler(uint32_t register_id, D3D12_SAMPLER_DESC const& desc, D3D12_SHADER_VISIBILITY visibility)
{
	ASSERT(init_static_samplers_num_ < samplers_num_);
	D3D12_STATIC_SAMPLER_DESC& static_desc = sampler_array_[init_static_samplers_num_++];
	static_desc.Filter = desc.Filter;
	static_desc.AddressU = desc.AddressU;
	static_desc.AddressV = desc.AddressV;
	static_desc.AddressW = desc.AddressW;
	static_desc.MipLODBias = desc.MipLODBias;
	static_desc.MaxAnisotropy = desc.MaxAnisotropy;
	static_desc.ComparisonFunc = desc.ComparisonFunc;
	static_desc.MinLOD = desc.MinLOD;
	static_desc.MaxLOD = desc.MaxLOD;

	//特殊字段BorderColor（类型不一致）
	if (static_desc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER
		|| static_desc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER
		|| static_desc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
	{
		//TODO:assert
		if (desc.BorderColor[3] == 1.0f)
		{
			if (desc.BorderColor[0] == 1.0f)
			{
				static_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			}
			else
			{
				static_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			}
		}
		else
		{
			static_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		}
	}

	//特有的字段
	static_desc.ShaderRegister = register_id;
	static_desc.RegisterSpace = 0;
	static_desc.ShaderVisibility = visibility;
}

void root_signature::finalize(wchar_t const* name, D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	if (finalized_) { return; }

	ASSERT(samplers_num_ == init_static_samplers_num_);

	D3D12_ROOT_SIGNATURE_DESC root_desc;
	root_desc.NumParameters = parameters_num_;
	root_desc.pParameters = param_array_.get();
	root_desc.NumStaticSamplers = samplers_num_;
	root_desc.pStaticSamplers = sampler_array_.get();
	root_desc.Flags = flags;

	descriptor_table_bmp_ = 0;
	sampler_table_bmp_ = 0;

	size_t hash_code = utility::hash_array(&root_desc.Flags, 1);
	hash_code = utility::hash_array(root_desc.pStaticSamplers, root_desc.NumStaticSamplers, hash_code);

	for (UINT i = 0; i < root_desc.NumParameters; ++i)
	{
		descriptor_table_size_[i] = 0;

		D3D12_ROOT_PARAMETER const& param = root_desc.pParameters[i];
		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			hash_code = utility::hash_array(param.DescriptorTable.pDescriptorRanges, param.DescriptorTable.NumDescriptorRanges, hash_code);

			if (param.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
			{
				sampler_table_bmp_ |= (1 << i);
			}
			else
			{
				descriptor_table_bmp_ |= (1 << i);
			}

			for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
			{
				descriptor_table_size_[i] += param.DescriptorTable.pDescriptorRanges[j].NumDescriptors;
			}
		}
		else
		{
			hash_code = utility::hash_array(&param, 1, hash_code);
		}
	}

	//TODO: 这里简化了，没有多线程支持

	auto iter = rhi_root_signatures_map_.find(hash_code);
	if (iter == rhi_root_signatures_map_.end())
	{
		ComPtr<ID3DBlob> blob, error_blob;
		ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1, blob.GetAddressOf(), error_blob.GetAddressOf()));
		ASSERT_SUCCEEDED(get_rhi()->device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rhi_root_signature_)));
		rhi_root_signature_->SetName(name);

		rhi_root_signatures_map_[hash_code] = rhi_root_signature_;
	}
	else
	{
		rhi_root_signature_ = iter->second.Get();
	}

	finalized_ = true;
}
