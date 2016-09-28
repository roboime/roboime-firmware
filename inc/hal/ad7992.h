#pragma once
#include <hal/i2c.h>
#include <inttypes.h>
class AD7992{
public:
	AD7992(I2C &i2c, uint8_t address):
		_i2c(i2c),
		_address(address)
	{
	}

	float Read(uint8_t channel){
		uint16_t reg = 0;
		float res = 0.0f;
		reg = _i2c.ReadRegHalfWord(_address, (channel & 0x03)<<4);
		reg&=0x0FFF;
		res = (float)(reg)/4095.0f*5.0f;
		return res;
	}
	uint8_t SelfTest(){
		uint8_t cfg=_i2c.ReadRegByte(_address, 0x02);
		if(cfg==0x08){
			return 0;
		}
		return 1;
	}
protected:
	I2C &_i2c;
	uint8_t _address;
};
