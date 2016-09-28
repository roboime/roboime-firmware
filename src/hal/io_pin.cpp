#include <hal/io_pin.h>

#include "stm32f4xx_rcc.h"

IO_Pin::IO_Pin(IO_Pin_Mode mode):
	_mode(mode)
{
}
