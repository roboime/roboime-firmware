#pragma once
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_rng.h>

class RANDOM_STM32{
public:
	static uint32_t GetRandomNumber(){
		if(!_initialized){
			RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
			RNG_Cmd(ENABLE);
			_initialized=1;
		}
		return RNG_GetRandomNumber();
	}
protected:
	static uint8_t _initialized;

};

uint8_t RANDOM_STM32::_initialized=0;
