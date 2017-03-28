#include <hal/ina220.h>

INA220::INA220(I2C &i2c, uint8_t address) :
		_i2c(&i2c), _address(address) {
}

float INA220::ReadBusVoltage() {
	uint16_t reg = 0;
	float res = 0.0f;
	reg = _i2c->ReadRegHalfWord(_address, 0x02);
	res = (float) (reg >> 3) * 0.004f;
	return res;
}

float INA220::ReadCurrent() {
	uint16_t reg = 0;
	float res = 0.0f;
	reg = _i2c->ReadRegHalfWord(_address, 0x04);
	res = (float) (reg) / 10000.0f;
	return res;
}

float INA220::ReadPower() {
	uint16_t reg = 0;
	float res = 0.0f;
	reg = _i2c->ReadRegHalfWord(_address, 0x03);
	res = (float) (reg)/500.0f;
	return res;
}

uint8_t INA220::SelfTest() {
	uint8_t temp[2] = { 0x80, 0x00 };
	uint8_t temp2[2];
	_i2c->WriteRegBuffer(_address, 0x00, temp, 2);
	_i2c->ReadRegBuffer(_address, 0x00, temp2, 2);
	if (temp2[0] == 0x39 && temp2[1] == 0x9f) {
		temp[0] = 0x06;
		temp[1] = 0x67;
		_i2c->WriteRegBuffer(_address, 0x00, temp, 2);
		_i2c->ReadRegBuffer(_address, 0x00, temp2, 2);
		temp[0] = 0x35;
		temp[1] = 0x55;
		_i2c->WriteRegBuffer(_address, 0x05, temp, 2);
		_i2c->ReadRegBuffer(_address, 0x05, temp2, 2);
		return 0;
	} else {
		return 1;
	}
}
