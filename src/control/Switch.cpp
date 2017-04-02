/*
 * switch.cpp
 *
 *  Created on: 1 de abr de 2017
 *      Author: OniasC
 */

#include <Switch.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"



static __IO uint32_t TimingDelay;


Switch::Switch(IO_Pin_STM32 sw1, IO_Pin_STM32 sw2, IO_Pin_STM32 sw3)
{
	SysTick_Config(SystemCoreClock/1000);

	sw1.Set();
	sw2.Set();
	sw3.Set();

	status[0]=sw1.Read();
	status[1]=sw2.Read();
	status[2]=sw3.Read();

	for (int i=0;i<3;i++){
		if(status[i]==Bit_SET) idbin[i]=true;
		else idbin[i]=false;
	}
	id=0;
	id = (uint8_t)idbin[0]+(uint8_t)2*idbin[1]+(uint8_t)4*idbin[2];
}

