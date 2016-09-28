#pragma once

#include <hal/io_pin.h>
#include <hal/ds2413.h>


class IO_Pin_DS2413:public IO_Pin
{
public:
	typedef enum{
		DS2413_GPIO_PinA=0x01,
		DS2413_GPIO_PinB=0x02,

	} DS2413_GPIO_Pin;

	IO_Pin_DS2413(IO_Pin_Mode mode, DS2413 &ds2413, uint8_t pins);
	uint8_t Read();
	void Write(uint8_t value);
	void Set();
	void Reset();
	void Interrupt(uint8_t newstate);

protected:
	DS2413 *_ds2413;
	uint8_t _pins;
	uint8_t _value;
};


