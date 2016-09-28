#pragma once
#include <hal/i2c.h>
#include <hal/memory.h>

class FM24V10: public MEMORY{
public:
	FM24V10(I2C &i2c, uint8_t address, uint32_t size, uint32_t startaddress=0);
	uint8_t SelfTest();
	uint8_t Read(uint32_t address, uint8_t *dest, uint32_t size);
	uint8_t Write(uint32_t address, uint8_t *source, uint32_t size);
protected:
	I2C *_i2c;
	uint8_t _address;
	uint32_t _startaddress;
	uint32_t _size;
};
