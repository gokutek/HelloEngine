#include "root_signature.h"
#include "utility.h"

class root_parameter : public D3D12_ROOT_PARAMETER
{
public:
};

root_signature::root_signature(uint32_t root_params_num, uint32_t static_samplers_num) :
	finalized_(false),
	parameters_num_(0),
	samplers_num_(0),
	init_static_samplers_num_(0),
	descriptor_table_bmp_(0),
	sampler_table_bmp_(0),
	rhi_root_signature_(nullptr)
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
	//TODO:
}

void root_signature::reset(uint32_t root_params_num, uint32_t static_samplers_num)
{
	parameters_num_ = root_params_num;
	samplers_num_ = static_samplers_num;
	init_static_samplers_num_ = 0;
	param_array_.reset(new root_parameter[root_params_num]);
	sampler_array_.reset(new D3D12_STATIC_SAMPLER_DESC[static_samplers_num]);
}

root_parameter* root_signature::get_root_parameter(size_t index)
{
	//TODO:
	return nullptr;
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

	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = parameters_num_;
	desc.pParameters = param_array_.get();
	desc.NumStaticSamplers = samplers_num_;
	desc.pStaticSamplers = sampler_array_.get();
	desc.Flags = flags;

	descriptor_table_bmp_ = 0;
	sampler_table_bmp_ = 0;

	//TODO:
}
