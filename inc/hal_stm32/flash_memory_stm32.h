#pragma once
#include <hal/memory.h>

class FLASH_MEMORY_STM32: public MEMORY{
public:
	FLASH_MEMORY_STM32(uint32_t addressbegin, uint32_t size):
		MEMORY(size),
		_addressbegin(addressbegin),
		_size(size)
	{
	}
	uint8_t *GetPointer(){
		return (uint8_t*)_addressbegin;
	}
protected:
	uint32_t _addressbegin;
	uint32_t _size;
};
