#pragma once

#include "pch.h"
#include "gpu_buffer.h"

/*
===============================================================================
===============================================================================
*/
class byte_address_buffer : public gpu_buffer
{
public:

protected:
    virtual void create_derived_views() override;
};
