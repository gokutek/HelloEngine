#include "root_parameter.h"
#include "utility.h"

root_parameter::root_parameter()
{
	ZeroMemory(&rhi_root_parameter_, sizeof(rhi_root_parameter_));
	rhi_root_parameter_.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)-1;
}

root_parameter::~root_parameter()
{
	clear();
}

void root_parameter::clear()
{
	if (rhi_root_parameter_.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
	{
		delete[] rhi_root_parameter_.DescriptorTable.pDescriptorRanges;
	}

	rhi_root_parameter_.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)-1;
}

void root_parameter::init_as_constants(UINT shader_register, UINT num_dwords, D3D12_SHADER_VISIBILITY shader_visibility, UINT register_space)
{
	rhi_root_parameter_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rhi_root_parameter_.ShaderVisibility = shader_visibility;
	rhi_root_parameter_.Constants.ShaderRegister = shader_register;
	rhi_root_parameter_.Constants.RegisterSpace = register_space;
	rhi_root_parameter_.Constants.Num32BitValues = num_dwords;
}

void root_parameter::init_as_constant_buffer(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility, UINT register_space)
{
	rhi_root_parameter_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rhi_root_parameter_.ShaderVisibility = shader_visibility;
	rhi_root_parameter_.Descriptor.ShaderRegister = shader_register;
	rhi_root_parameter_.Descriptor.RegisterSpace = register_space;
}

void root_parameter::init_as_buffer_srv(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility, UINT register_space)
{
	rhi_root_parameter_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rhi_root_parameter_.ShaderVisibility = shader_visibility;
	rhi_root_parameter_.Descriptor.ShaderRegister = shader_register;
	rhi_root_parameter_.Descriptor.RegisterSpace = register_space;
}

void root_parameter::init_as_buffer_uav(UINT shader_register, D3D12_SHADER_VISIBILITY shader_visibility, UINT register_space)
{
	rhi_root_parameter_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rhi_root_parameter_.ShaderVisibility = shader_visibility;
	rhi_root_parameter_.Descriptor.ShaderRegister = shader_register;
	rhi_root_parameter_.Descriptor.RegisterSpace = register_space;
}

void root_parameter::init_as_descriptor_table(UINT num_descriptor_ranges, D3D12_SHADER_VISIBILITY shader_visibility)
{
	rhi_root_parameter_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rhi_root_parameter_.ShaderVisibility = shader_visibility;
	rhi_root_parameter_.DescriptorTable.NumDescriptorRanges = num_descriptor_ranges;
	ASSERT(rhi_root_parameter_.DescriptorTable.pDescriptorRanges == nullptr);
	rhi_root_parameter_.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[num_descriptor_ranges];
}

void root_parameter::set_table_range(UINT range_index, D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT count, UINT register_space)
{
	D3D12_DESCRIPTOR_RANGE& range = (D3D12_DESCRIPTOR_RANGE&)rhi_root_parameter_.DescriptorTable.pDescriptorRanges[range_index];
	range.RangeType = range_type;
	range.NumDescriptors = count;
	range.BaseShaderRegister = base_shader_register;
	range.RegisterSpace = register_space;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void root_parameter::init_as_descriptor_range(D3D12_DESCRIPTOR_RANGE_TYPE range_type, UINT base_shader_register, UINT count, D3D12_SHADER_VISIBILITY shader_visibility, UINT register_space)
{
	init_as_descriptor_table(1, shader_visibility);
	set_table_range(0, range_type, base_shader_register, count, register_space);
}
