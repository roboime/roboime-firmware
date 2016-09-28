#pragma once

#include <hal/i2c.h>

class INA220{
public:
	INA220(I2C &i2c, uint8_t address);
	float ReadBusVoltage();
	float ReadCurrent();
	float ReadPower();
	uint8_t SelfTest();
protected:
	I2C *_i2c;
	uint8_t _address;
};
