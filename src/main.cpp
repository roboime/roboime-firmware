/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **  Distribution: The file is distributed "as is", without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. This file may only be built (assembled or compiled and linked)
 **  using the Atollic TrueSTUDIO(R) product. The use of this file together
 **  with other tools than Atollic TrueSTUDIO(R) is not permitted.
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include <utils/time_functions.h>
#include "GPIO.h"
#include "hal_stm32/io_pin_stm32.h"

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

#include <stm32f4xx_wwdg.h>

int main(void){

	SysTick_Config(SystemCoreClock/1000);

	bool id[3];
	uint8_t status[3];

	IO_Pin_STM32 sw1(IO_Pin::IO_Pin_Mode_IN, GPIOD, GPIO_Pin_6, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 sw2(IO_Pin::IO_Pin_Mode_IN, GPIOD, GPIO_Pin_7, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 sw3(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_PP);

	/*GPIO sw1(GPIOE, GPIO_Pin_2);
	GPIO sw2(GPIOD, GPIO_Pin_7);
	GPIO sw3(GPIOD, GPIO_Pin_6);
	sw1.Set();
	sw2.Set();
	sw3.Set();
	id[0] = sw1.Status();
	id[1] = sw2.Status();
	id[2] = sw3.Status();*/

	sw1.Set();
	sw2.Set();
	sw3.Set();

	status[0]=sw1.Read();
	status[1]=sw2.Read();
	status[2]=sw3.Read();

	for (int i=0;i<3;i++){
		if(status[i]==Bit_SET) id[i]=true;
		else id[i]=false;
	}

	id[0];
	id[1];
	id[2];

	delay_ms(3);
	while(1){


	}

}

extern uint32_t LocalTime;

extern "C" void SysTick_Handler(void){
	//	TimingDelay_Decrement();
	LocalTime++;
}

extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
	/* TODO, implement your code here */
	return -1;
}
