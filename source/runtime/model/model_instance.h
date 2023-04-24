#pragma once

#include "core/pch.h"
#include <memory>

class model;

class model_instance
{
public:

private:
	std::shared_ptr<const model> model_;
	//UploadBuffer m_MeshConstantsCPU;
	//ByteAddressBuffer m_MeshConstantsGPU;
};
