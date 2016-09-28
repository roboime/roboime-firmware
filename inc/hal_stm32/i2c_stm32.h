#pragma once

#include <hal/i2c.h>

#include <stm32f4xx_i2c.h>
#include <stm32f4xx_gpio.h>


class I2C_STM32: public I2C{
public:
	I2C_STM32(IO_Pin &sdapin, IO_Pin &sclpin, I2C_TypeDef* i2c, uint32_t speed, uint32_t timeout);
	uint8_t WriteRegBuffer (uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size);
	uint8_t ReadRegBuffer(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size);
	uint8_t WriteReg16Buffer (uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size);
	uint8_t ReadReg16Buffer(uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size);

protected:
	I2C_TypeDef* _i2c;
	uint32_t _speed;
	uint32_t _timeout;
	void Config();
	uint8_t Timeout();

};

