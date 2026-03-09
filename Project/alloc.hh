#pragma once

#include <stdint.h>

namespace oldking 
{
	void* alloc(uint32_t size);

	void dealloc(void* obj, uint32_t size);
}

