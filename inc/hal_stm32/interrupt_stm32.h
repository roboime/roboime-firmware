#pragma once

#include <misc.h>

class INTERRUPT_STM32{
public:
	INTERRUPT_STM32(uint8_t IRQChannel, uint8_t IRQChannelPreemptionPriority=0x0F, uint8_t IRQChannelSubPriority=0x0F, uint8_t IRQChannelCmd=ENABLE):
		_IRQChannel(IRQChannel)
	{
		if(IRQChannel){
			NVIC_InitTypeDef NVIC_InitStructure;
			NVIC_InitStructure.NVIC_IRQChannel=IRQChannel;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=IRQChannelPreemptionPriority;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority=IRQChannelSubPriority;
			NVIC_InitStructure.NVIC_IRQChannelCmd=IRQChannelCmd?ENABLE:DISABLE;
			NVIC_Init(&NVIC_InitStructure);
		}
	}
protected:
	uint8_t _IRQChannel;
};
