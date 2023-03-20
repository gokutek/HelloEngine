#pragma once

#include "pch.h"
#include <stdint.h>

class command_context
{
public:
	static command_context& begin(const wchar_t* ID);
	~command_context();

	uint64_t flush(bool wait_for_completion);
	uint64_t finish(bool wait_for_completion);
	
	void initialize();

	//TODO:��������ת�͵ķ���

	ID3D12GraphicsCommandList* get_command_list();

	//TODO:����buffer�Ŀ����ķ���

private:
	command_context();
};
