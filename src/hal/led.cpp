#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

#include <hal/led.h>

LED::LED(IO_Pin *pin):
	_pin(pin)
{
}

void LED::On()
{
	_pin->Set();
}

void LED::Off()
{
	_pin->Reset();
}

void LED::Toggle()
{
	if(_pin->Read()){
		_pin->Reset();
	} else 	{
		_pin->Set();
	}
}

uint8_t LED::Read(){
	return _pin->Read();
}
