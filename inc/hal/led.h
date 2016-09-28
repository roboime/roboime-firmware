#pragma once

#include <hal/io_pin.h>

class LED
{
public:
	LED(IO_Pin *pin);
	void On();
	void Off();
	void Toggle();
	uint8_t Read();
protected:
	IO_Pin *_pin;
};


