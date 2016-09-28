#pragma once
#include <hal/i2c.h>

class TMP102{
public:
	TMP102(I2C &i2c, uint8_t address);
	uint8_t SelfTest();
protected:
	I2C *_i2c;
	uint8_t _address;
};
