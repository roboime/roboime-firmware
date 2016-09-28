#pragma once
#include <hal/i2c.h>
#include <hal/io_pin.h>

class MCP4XXX{
public:
	MCP4XXX(I2C &i2c, uint8_t address, IO_Pin &vddpin=*(IO_Pin*)0);
	uint8_t SelfTest();
	void VDD(uint8_t state);
	void SetValue(uint8_t value);
	uint8_t GetValue();

protected:
	I2C *_i2c;
	uint8_t _address;
	IO_Pin *_vddpin;
};


