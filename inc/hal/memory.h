#pragma once
#include <inttypes.h>

class MEMORY{
public:
	MEMORY(uint32_t size):_size(size){}
	virtual uint8_t Read(uint32_t address, uint8_t *dest, uint32_t size){
		return 0;
	}

	virtual uint8_t Write(uint32_t address, uint8_t *dest, uint32_t size){
		return 0;
	}
	virtual uint8_t *GetPointer(){
		return 0;
	}
protected:
	uint32_t _size;
};
